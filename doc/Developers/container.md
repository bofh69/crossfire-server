# Container image targets

The `Containerfile` has two targets:

- `build`: compiles and installs Crossfire into `/tmp/crossfire-install/usr/local`.
- `deployment`: copies the installed files into the runtime image and starts crossfire via an entrypoint script.

Build the deployment image with:

```sh
podman build --target deployment -t crossfire-server .
```

## Required volumes

The deployment image requires these mount points:

- `/usr/local/share/crossfire/arch` (external archetype data, mount read-only)
- `/usr/local/share/crossfire/maps` (external maps data, mount read-only)
- `/usr/local/etc/crossfire` (server configuration)
- `/usr/local/var/crossfire` (persistent runtime state, read/write)

The image explicitly declares writable volumes for `/usr/local/etc/crossfire` and `/usr/local/var/crossfire`.

On first start the entrypoint script (`utils/crossfire-entrypoint.sh`
in the repository, installed as `/usr/local/bin/crossfire-entrypoint.sh`)
copies any missing files from `/usr/local/share/crossfire-example`
(the bundled example configuration) into `/usr/local/etc/crossfire`, then
starts `/usr/local/bin/crossfire-server`.  Files that already exist in the volume are left untouched, so manual
customisations survive container restarts and upgrades.

No manual volume pre-population is required; simply mount the volumes and start the container:

```sh
podman volume create crossfire-etc
podman volume create crossfire-var
```

Then edit files under `crossfire-etc` as needed (the example defaults are placed there automatically on first run). After the first run crossfire-etc can be mounted readonly.

## Example run command

```sh
podman run -u $UID --replace --name crossfire-server \
  -p 13327:13327/tcp \
  -v /path/to/arch:/usr/local/share/crossfire/arch:ro,z \
  -v /path/to/maps:/usr/local/share/crossfire/maps:ro,z \
  -v crossfire-etc:/usr/local/etc/crossfire:Z \
  -v crossfire-var:/usr/local/var/crossfire:Z \
  crossfire-server
```
