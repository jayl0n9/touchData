# touchData

在windows系统写文件时，总是为修改文件时间而感到烦恼，在webshell权限不足时，需要用命令进行修改未免太过麻烦。它不像linux一样，touch -r 即可复制其他文件时间

于是便写了这个小工具.考虑到编译大小，最后还是选择了c++进行编写。200多k，符合我的预期，在使用时就比较方便

用法：
~~~
touchData.exe -f <target file path> -r <reference file path> -d <time>
~~~

示例：
~~~
touchData.exe -f aaa.txt -d "2022-11-11 11:11:11"    #将aaa.txt的创建时间、修改时间、访问时间，设置为：2022-11-11 11:11:11

touchData.exe -f aaa.txt -r "bbb.txt"                #将aaa.txt的创建时间、修改时间、访问时间，设置为bbb.txt的创建时间、修改时间、访问时间
~~~
