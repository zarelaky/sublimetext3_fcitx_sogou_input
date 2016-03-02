# sublimetext3_sogou_input
library for sublimetext 3 inputing chinese with sogou pinyin im

##Changelog
1. fixed sublimetext 3 failed when execute "Preferences"->"Browse Packages"
2. fixed "Open Terminal Here" failed 

##Testing
The code has been tested in env below:
    ubuntu 14.04 x64
    sublimetext 3107 x64
    sogou input for linux 2.0.0.0068 x64

##History
The code is based 'sublime_imfix.c' written by Cjacker Huang <jianzhong.huang at i-soft.com.cn>

##How to compile it
```
gcc -shared -o libsublime-imfix.so sublime_imfix.c  `pkg-config --libs --cflags gtk+-2.0` -fPIC
```
##How to use it
```
LD_PRELOAD=./libsublime-imfix.so sublime_text 
```
or 
create scripts instead of offical one.
My scripts subl.sh is below, as a example.

```
#!/bin/bash 
SUBLIME_VERSION=3107
SUBLIME_TEXT_DIR=/usr/local/sublime_text
IMFIX_SO=libsublime-imfix.so
LD_PRELOAD=${SUBLIME_TEXT_DIR}/${IMFIX_SO} \
    ${SUBLIME_TEXT_DIR}/${SUBLIME_VERSION}/sublime_text_3/sublime_text $@
```
```ln -sf <path_to_subl.sh> /usr/bin/subl```

Do not forget change you sublime_text.desktop file
Below is mine.
```
[Desktop Entry]
Version=1.0
Type=Application
Name=Sublime Text
GenericName=Text Editor
Comment=Sophisticated text editor for code, markup and prose
Exec=/usr/local/sublime_text/subl.sh %F
Terminal=false
MimeType=text/plain;
Icon=/usr/local/sublime_text/sublime-text.png
Categories=TextEditor;Development;
StartupNotify=true
Actions=Window;Document;

[Desktop Action Window]
Name=New Window
Exec=/usr/local/sublime_text/subl.sh -n
OnlyShowIn=Unity;

[Desktop Action Document]
Name=New File
Exec=/usr/local/sublime_text/subl.sh --command new_file
OnlyShowIn=Unity;
```

All done, Good luck :~)
