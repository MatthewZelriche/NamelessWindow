~~1. Fix bug where cursor doesn't always get repositioned within a window immediately after the cursor is bound.~~<br />
1a. Even when cursor gets repositioned within window, first movement causes an instant "jump" of cursor pos.<br />
~~2. Fix bug where minimizing a window doesn't unbind master cursor.~~<br />
~~3. Figure out why xcb_xfixes_show_cursor sometimes has to be called many times.~~<br />
4. Attempting to construct a window on a second monitor on i3 (and probably other tiling managers) fails.<br />
~~5. On i3, forcing a window to floating results in the pointer not being grabbed.~~<br />
6. If a resize is performed and the cursor falls outside the area of the new size, the mouse is ungrabbed.<br />