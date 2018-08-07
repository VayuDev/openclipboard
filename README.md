# Print clipboard to stdout

Prints current clipboard content to stdout, checkes for new content every 0.25 seconds.

Works with XServer and Xlib. Compile with "gcc -O3 main.c -lX11 -o openclipboard".

A bit of code stolen from [here](https://www.uninformativ.de/blog/postings/2017-04-02/0/POSTING-en.html).