Git Guidelines {#git}
==============
1. All commits should include a log message. The first line of the commit
   should begin with a capital letter and summarize the changes in about
   50 characters. More detailed explanatory text, if necessary, should be
   separated by an empty line and be wrapped at around 72 characters.

   It is not necessary to go into a long exposition, and pasting the actual
   changes is not generally useful.  But this log message should be useful for
   someone looking over the logs at a future point to see what did change.
   Having a log like 'various skill stuff' isn't very useful.  A log message like
   'prevent abuse with the literacy skill, and increase chance of singing' is
   much more useful, and not a lot more words.

   One of the main uses of the log entries is when bugs are reported where
   behaviour changed between version X and Y to be able to look at the log
   entries and get an idea of what specific revision may have caused that
   change.

   Do not refer to other files or other log messages.  Saying 'see changes
   file' is not useful, nor is a message like 'continuing with last set
   of commits'.  Such messages are not useful when trying to look back through
   the logs at a future point.

2. Update the ChangeLog for important, user-visible changes that are suitable
   for inclusion in a release's change log.

3. All commits should go through at least minimum testing:
   For source code, this means it compiles and at least a basic test has
   been done (for example, if it is a new spell, have you tried casting
   the spell?)  This basic testing implies the code at least compiles
   also.  I realize it is very difficult to do 100% testing of code, but
   at least a basic test should be done.

   For archetypes, this testing should involve rebuilding the arch file and
   running with the new file.  There should be no errors in the loading
   of the archetype files.

   For maps, this means that the map should load, and the exits should lead
   back and forth.  Note that maps in the unlinked directory are more
   work in progress so can be checked in a more experimental state.

4. All source code should be C99 compliant. Be careful of new library calls
   that are not being used elsewhere in the source - there may be a reason they
   are not being used.  "it compiles on my system" is not justification for
   writing code that does not work elsewhere.  It is understandable that you may
   not know that the code written is non portable, but once this is learned, it
   should be corrected.

5. Style & Balance: Your changes may work, but do they fit in with the
   rest of the game.  This basically means following the files guides
   that already existing, eg doc/programming_guide, doc/mapguide

   There really is no arch guide, but take common sense.  Does the
   object fit in with the game (ie, a blaster rifle would not), is this
   arch very unbalancing, etc.

6. Before starting a big project, send a note to the mailing list asking
   for opinions.  While it is not possible to prevent someone working on
   whatever they may want, if the general consensus is that it is a bad idea,
   you may want to find that out before spending a lot of work on it only
   to find out that your idea will not get added to the game.

7. Take responsibility for your code.  If you check in something and a bug
   is reported in it, go an fix it.
