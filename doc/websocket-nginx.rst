Crossfire WebSocket – nginx Reverse Proxy Setup
================================================

This guide explains how to put nginx in front of the Crossfire WebSocket
server so that:

* The WebSocket port is exposed on the standard HTTPS port (443), letting
  browser-based clients connect through firewalls that block non-standard
  ports.
* TLS is terminated by nginx using a free Let's Encrypt certificate, so
  browsers can use ``wss://`` (WebSocket over TLS).
* The real client IP address is forwarded to Crossfire via the
  ``X-Forwarded-For`` header so that the IP ban list (``checkbanned``)
  is enforced on the originating client rather than on the proxy itself.


Prerequisites
-------------

* A Linux server with a public IP address.
* A domain name pointing to that IP (e.g. ``crossfire.example.com``).
* nginx ≥ 1.3.13 (WebSocket proxying support).
* Certbot for Let's Encrypt certificate management.
* Crossfire server built with WebSocket support, started with ``-ws-port``::

    $ crossfire-server -ws-port 8080


Install nginx and Certbot
-------------------------

On Debian / Ubuntu::

    sudo apt update
    sudo apt install nginx certbot python3-certbot-nginx

On RHEL / Fedora::

    sudo dnf install nginx certbot python3-certbot-nginx


Obtain a Let's Encrypt Certificate
-----------------------------------

Replace ``crossfire.example.com`` with your actual domain::

    sudo certbot --nginx -d crossfire.example.com

Certbot will:

1. Verify domain ownership by writing a temporary file served by nginx.
2. Download a certificate and private key to ``/etc/letsencrypt/live/``.
3. Automatically edit the nginx configuration to enable HTTPS.
4. Install a cron job / systemd timer to renew the certificate before it
   expires (Let's Encrypt certificates are valid for 90 days).

You can verify automatic renewal at any time with::

    sudo certbot renew --dry-run


nginx Configuration
-------------------

Create or replace ``/etc/nginx/sites-available/crossfire-ws`` with the
following.  Adjust the values marked ``# ← change me`` to match your
environment::

    # /etc/nginx/sites-available/crossfire-ws

    # Redirect plain HTTP to HTTPS.
    server {
        listen 80;
        server_name crossfire.example.com;   # ← change me

        # Allow Certbot ACME challenges.
        location /.well-known/acme-challenge/ {
            root /var/www/html;
        }

        location / {
            return 301 https://$host$request_uri;
        }
    }

    # HTTPS + WebSocket proxy.
    server {
        listen 443 ssl;
        server_name crossfire.example.com;   # ← change me

        # Let's Encrypt certificate (filled in by certbot --nginx).
        ssl_certificate     /etc/letsencrypt/live/crossfire.example.com/fullchain.pem;  # ← change me
        ssl_certificate_key /etc/letsencrypt/live/crossfire.example.com/privkey.pem;    # ← change me

        # Modern TLS settings.
        ssl_protocols TLSv1.2 TLSv1.3;
        ssl_prefer_server_ciphers on;
        ssl_session_cache shared:SSL:10m;

        # Optional: serve a web-based Crossfire client from this directory.
        # root /var/www/crossfire-client;
        # index index.html;

        # WebSocket proxy.
        location /ws {
            proxy_pass http://127.0.0.1:8080;  # ← Crossfire ws-port

            # Required WebSocket upgrade headers.
            proxy_http_version 1.1;
            proxy_set_header Upgrade    $http_upgrade;
            proxy_set_header Connection "Upgrade";

            # Forward the real client IP to Crossfire.
            # The server reads this header and checks it against the ban list.
            proxy_set_header X-Forwarded-For $remote_addr;

            # Pass through the Host header.
            proxy_set_header Host $host;

            # Increase timeout for long-lived WebSocket connections.
            proxy_read_timeout  3600s;
            proxy_send_timeout  3600s;
        }
    }

Enable the site and reload nginx::

    sudo ln -s /etc/nginx/sites-available/crossfire-ws \
               /etc/nginx/sites-enabled/crossfire-ws
    sudo nginx -t          # syntax check
    sudo systemctl reload nginx


Security Notes
--------------

Trust the X-Forwarded-For header only from the proxy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``X-Forwarded-For`` header can be forged by any client that connects
directly to the Crossfire WebSocket port, bypassing the proxy.  You can
prevent such direct connections with a firewall rule::

    # Block direct external access to the WebSocket port.
    sudo ufw deny 8080/tcp

Only nginx (running on the same machine) needs to reach port 8080.

If you choose not to use a firewall, direct connections are still subject
to the normal ``checkbanned`` check on the raw TCP peer address — the same
protection applied to the standard Crossfire port.  In that case the
``X-Forwarded-For`` header will simply be absent and the ban check falls
back to the TCP address, exactly as it would for a plain TCP connection.

How Crossfire uses X-Forwarded-For
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the Crossfire WebSocket server receives a connection it always checks
the raw TCP peer address against ``checkbanned`` first.  If the HTTP
``Upgrade`` request also contains an ``X-Forwarded-For`` header the server:

1. Extracts the **first** (leftmost) IP address from the header value.
2. Validates that the address contains only legal IP characters to prevent
   log-injection attacks.
3. Checks the address against ``checkbanned``.
4. If the address is banned, the connection is closed immediately.
5. If the address is allowed, ``ns->host`` is updated to the forwarded
   address so that all subsequent server log messages and ban checks see
   the real client IP.

Verify the setup
~~~~~~~~~~~~~~~~

After starting nginx and Crossfire you can test the WebSocket connection
from your browser's developer console::

    const ws = new WebSocket('wss://crossfire.example.com/ws');
    ws.onopen = () => console.log('Connected');
    ws.onerror = (e) => console.error('Error', e);

Or from the command line with ``websocat``::

    websocat wss://crossfire.example.com/ws


Serving a Web-Based Client
---------------------------

To serve a static web-based Crossfire client (HTML/JS/CSS files) from the
same nginx instance:

1. Copy the client files to ``/var/www/crossfire-client/``.
2. Uncomment the ``root`` and ``index`` lines in the nginx configuration
   above.
3. Add a ``location /`` block in the ``server { listen 443 … }`` block::

       location / {
           try_files $uri $uri/ =404;
       }

Browsers will then be able to:

* Download the client over ``https://crossfire.example.com/``
* Connect to the game server over ``wss://crossfire.example.com/ws``

Both connections are encrypted with the same Let's Encrypt certificate,
satisfying the browser's **mixed-content** security policy.


Firewall Summary
-----------------

=========  ======  ===================================================
Port       Proto   Purpose
=========  ======  ===================================================
80/tcp     TCP     HTTP (Certbot ACME + redirect to HTTPS)
443/tcp    TCP     HTTPS (nginx – serves client + proxies WebSocket)
``ws-port``  TCP  Crossfire WebSocket (optionally firewalled)
=========  ======  ===================================================

References
----------

* `nginx WebSocket proxying <https://nginx.org/en/docs/http/websocket.html>`_
* `Certbot documentation <https://certbot.eff.org/>`_
* `Let's Encrypt – Getting Started <https://letsencrypt.org/getting-started/>`_
* `RFC 6455 – The WebSocket Protocol <https://www.rfc-editor.org/rfc/rfc6455>`_
* `RFC 7239 – Forwarded HTTP Extension <https://www.rfc-editor.org/rfc/rfc7239>`_
