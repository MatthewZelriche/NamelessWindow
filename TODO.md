~~1. Fix bug where cursor doesn't always get repositioned within a window immediately after the cursor is bound.~~<br />
~~1a. Even when cursor gets repositioned within window, first movement causes an instant "jump" of cursor pos.~~<br />
~~2. Fix bug where minimizing a window doesn't unbind master cursor.~~<br />
~~3. Figure out why xcb_xfixes_show_cursor sometimes has to be called many times.~~<br />
4. Attempting to construct a window on a second monitor on i3 (and probably other tiling managers) fails.<br />
5. ~~On i3, forcing a window to floating results in the pointer not being grabbed.~~<br />
6. ~~If a resize is performed and the cursor falls outside the area of the new size, the mouse is ungrabbed.~~<br />
7. ~~Fix bug where cursor is not unhidden if a window is destroyed.~~ <br />
8. ~~If 2 windows overlap, the cursor is shown when it should be hidden.~~<br />
9. ~~If 2 windows overlap, and the focused (bottom) window is closed, the mouse remains hidden when leaving the overlapping window.~~<br />
10. ~~On i3, windows that are forced floating sometimes receive focus even when the cursor should be bound to the other window.~~<br />
11. Implement more examples.<br />
12. Look into supporting a larger number of mouse buttons on X11<br />
13. ~~Better method of determining key name on X11.~~<br />
14. Better method of enumerating keyboards on X11 (ignoring fake keyboards).<br />
15. ~~Certain modifiers (NumLock) are not handled globally on X11, but they are in Win32.~~<br />
16. ~~User-readable key names stored in KeyEvents are not uniform across platforms.~~<br />
17. ~~Fix fullscreen regression on X11, hopefully.~~<br />
18. Expose SetVideoMode to the public API.<br />
19. ~~Update X11 to confirm with RawMouse & Cursor API changes.~~<br />
20. Better GL Context creation on WIN32.<br />
21. Bug on i3 where cursor isn't rebound on focus via keyboard shortcut.<br />
22. ~~On Win32, BindToWindow() doesn't automatically switch focus, but it does on linux. Which behavior should be kept?~~<br />
23. ~~Implement ImGui character input on Linux.~~<br />
24. ~~Fix ImGui modifiers being incorrect.~~<br />
25. Currently, CDS_FULLSCREEN is used for reverting video mode changes on windows. This functionality, as far as I can tell, does not exist in X11. Consider a way to do this that works for both platforms.<br />
26. ~~Modify public API so that fullscreen and borderless are independent of each other.~~<br />
27. Should lock modifiers be global across all keyboards, or specific to each physical device?
28. Win32: When clicking the window in the menubar to un-minimize, bind does not take effect.