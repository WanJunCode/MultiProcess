# MultiProcess
Linux c++ multiprocess server framework

# C++ LINUX 平台下的一个多进程服务器实例

class MultiProcess 用于对多个进程的管理，在父进程中初始化，获得父进程的 pid 。

class Process 代表一个子进程

总体上是 MultiProcess 类，使用一个 idmap 容器，管理所有的子进程。

MultiProcess 使用方法
1. chkServerRunWpid() 创建一个 pid 记录文件
2. fork() 创建子进程任务，并且将该进程任务添加到 idmap 中
3. checkChildren() 检查所有的子进程，如果有进程发生异常退出了，则重新根据该进程的执行函数创建一个新的子进程。
4. killChildren() 销毁所有的子进程
5. killPid(pid_t pid) 销毁指定的子进程
6. listPid() 列出所有的子进程pid

父进程对子进程的控制，使用信号和 pid 来激活。

3.26日 改用 clang 进行编译
根据 WJ 添加 base 库