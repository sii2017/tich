# 第一章
## Win32基本程序概念   
在学习MFC之前，必要的基础是，对于Windows程序的事件驱动特性的了解（包括消息的产生、获得、分派、判断、处理），以及对C++ 多态（polymorphism）的精确体会。   
![](https://github.com/sii2017/image/blob/master/%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91%E6%B5%81%E7%A8%8B.jpg)   
### 以消息为基础，以事件驱动之
Windows 程序的进行系依靠外部发生的事件来驱动。   
换句话说，程序不断等待（利用一个while循环），等待任何可能的输入，然后做判断，再做适当的处理。上述的「输入」是由操作系统捕捉到之后，以消息形式（一种数据结构）进入程序之中。    
操作系统如何捕捉外围设备（如键盘和鼠标）所发生的事件呢？USER 模块掌管各个外围的驱动程序，它们各有侦测回路。   
如果把应用程序获得的各种「输入」分类，可以分为由硬件装置所产生的消息，如鼠标移动或键盘被按下（放在系统队列system queue中），以及由Windows系统或其它Windows程序传送过来的消息。（放在程序队列application queue中）      
对应用程序来说，消息就是消息，来自哪里或放在哪里其实并没有太大区别，反正程序调用GetMessage API就取得一个消息，程序的生命靠它来推动。   
所有的GUI 系统，包括UNIX的X Window 以及OS/2 的Presentation Manager，都像这样，是以消息为基础的事件驱动系统。      
接受并处理消息的主角就是窗口。每一个窗口都应该有一个函数负责处理消息，程序员必须负责设计这个所谓的「窗口消息处理函数」。    
如果窗口获得一个消息，这个窗口函数必须判断消息的类别，决定处理的方式。（对于某些系统消息，也可以采用不处理来默认处理。）     
以下为窗口程序处理消息的关系图：  
![](https://github.com/sii2017/image/blob/master/message.png)  
### 注册类及窗口创建
在设置好各项窗口类参数后使用RegisterClass()函数来创建窗口类。有一些默认的窗口类如对话框窗口，输入框窗口等可以直接使用不用注册。   
然后使用CreateWindow函数来使用刚刚注册的窗口类，创建一个简单的基本窗口。   
### 消息循环
初始化工作完成后，WinMain进入消息循环：   
```c
while(GetMessage(&msg, ...))  
{   
	TranslateMessage(&msg);//将键盘消息进行转化   
	DispatchMessage(&msg);//分派消息给相应窗口    
}   
```    
其中的TranslateMessage 是为了将键盘消息转化，DispatchMessage 会将消息传给窗口函数去处理。  
消息发生之时，操作系统已根据当时状态，为它标明了所属窗口，而窗口所属之窗口类别又已经标示了窗口函数（也就是wc.lpfnWndProc 所指定的函数)，所以DispatchMessage自有脉络可寻。   
### 窗口消息处理函数
消息循环中的DispatchMessage 把消息分配到哪里呢？它透过USER 模块的协助，送到该窗口的窗口函数去了。窗口函数通常利用switch/case 方式判断消息种类，以决定处置方式。    
由于它是被Windows 系统所调用的（我们并没有在应用程序任何地方调用此函数），所以这是一种call back 函数，意思是指「在你的程序中，被Windows 系统调用」的函数。这些函数虽然由你设计，但是永远不会也不该被你调用，它们是为Windows系统准备的。  
注意，不论什么消息，都必须被处理，所以switch/case 指令中的default: 处必须调用DefWindowProc，这是Windows 内部预设的消息处理函数。   
窗口函数的wParam和lParam的意义，因消息之不同而异。wParam在16位环境中是16位，在32位环境中是32 位。因此，参数内容（格式）在不同操作环境中就有了变化。    
### 消息映射（Message Map)的雏形
以下将模拟MFC的消息映射表格。   
```c   
//定义部分
struct MSGMAP_ENTRY {    
	UINT nMessage;   
	LONG (*pfn)(HWND, UINT, WPARAM, LPARAM);    
};   
#define dim(x) (sizeof(x) / sizeof(x[0]))    

struct MSGMAP_ENTRY _messageEntries[] =   
{  
	WM_CREATE, OnCreate,   
	WM_PAINT, OnPaint,  
	WM_SIZE, OnSize,    
	WM_COMMAND, OnCommand,   
	WM_SETFOCUS, OnSetFocus,   
	WM_CLOSE, OnClose,  
	WM_DESTROY, OnDestroy,    
} ;   

struct MSGMAP_ENTRY _commandEntries =   
{   
	IDM_ABOUT, OnAbout,  
	IDM_FILEOPEN, OnFileOpen,  
	IDM_SAVEAS, OnSaveAs,   
} ;    

//窗口消息处理函数部分  
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam)   
{   
	int i;    
	for(i=0; i < dim(_messageEntries); i++)     
	{    
		if (message == _messageEntries[i].nMessage)   
		//根据位置，通过函数指针来调用struct中不同的函数
		return((*_messageEntries[i].pfn)(hWnd, message, wParam, lParam));   
	}    
	return(DefWindowProc(hWnd, message, wParam, lParam));   
}    


LONG OnCommand(HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam)   
{    
	int i;    
	for(i=0; i < dim(_commandEntries); i++)    
	{    
		if (LOWORD(wParam) == _commandEntries[i].nMessage)   
		return((*_commandEntries[i].pfn)(hWnd, message, wParam, lParam));   
	}   
	return(DefWindowProc(hWnd, message, wParam, lParam));   
}    

LONG OnCreate(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)  
{   
	//...   
}  

LONG OnAbout(HWND hWnd, UINT wMsg, UINT wParam, LONG lParam)   
{   
	//...  
}    
```   
这么一来，WndProc 和OnCommand 永远不必改变，每有新要处理的消息，只要在_messageEntries[ ] 和_commandEntries[ ] 两个数组中加上新元素，并针对新消息撰写新的处理例程即可。    
这种观念以及作法就是MFC 的Message Map 的雏形。MFC 把其中的动作包装得更好更精致（当然因此也就更复杂得多），成为一张庞大的消息地图；程序一旦获得消息，就可以按图上溯，直到被处理为止。    
### 对话框
![](https://github.com/sii2017/image/blob/master/dialog.jpg)     
### 程序的生命周期
>1. 程序初始化过程中调用CreateWindow，为程序建立了一个窗口，做为程序的萤幕舞台。CreateWindow 产生窗口之后会送出WM_CREATE 直接给窗口函数，后者于是可以在此时机做些初始化动作（例如配置内存、开文件、读初始资料...）。    
2. 程序活着的过程中，不断以GetMessage 从消息贮列中抓取消息。如果这个消息是WM_QUIT，GetMessage 会传回0 而结束while 循环，进而结束整个程序。    
3. DispatchMessage 透过Windows USER 模块的协助与监督，把消息分派至窗口函数。消息将在该处被判别并处理。    
4. 程序不断进行2和3的动作。     
5. 当使用者按下系统菜单中的Close 命令项，系统送出WM_CLOSE。通常程序的窗口函数不栏截此消息，于是DefWindowProc 处理它。    
6. DefWindowProc 收到WM_CLOSE 后， 调用DestroyWindow 把窗口清除。DestroyWindow 本身又会送出WM_DESTROY。     
7. 程序对WM_DESTROY 的标准反应是调用PostQuitMessage。     
8. PostQuitMessage 没什么其它动作，就只送出WM_QUIT 消息，准备让消息循环中的GetMessage 取得，如步骤2，结束消息循环。    

操作系统与应用程序职司不同，二者是互相合作的关系，所以必需各做各的份内事，并互以消息通知对方。   
如果不依据这个游戏规则，可能就会有麻烦产生。你可以作一个小实验，在窗口函数中拦截WM_DESTROY，但不调用PostQuitMessage。你会发现当选择系统菜单中的Close 时，屏幕上这个窗口消失了，（因为窗口摧毁及数据结构的释放是DefWindowProc 调用DestroyWindow 完成的），但是应用程序本身并没有结束（因为消息循环结束不了），它还留存在内存中。  
### 一个进程的诞生与死亡 
执行一个程序，必然就产生一个进程（process）。最直接的程序执行方式就是在shell中以鼠标双击某一个可执行文件图标（假设其为App.exe），执行起来的App 进程其实是shell 调用CreateProcess 激活的。    
让我们看看整个流程：    
1. shell 调用CreateProcess 激活App.exe。   
2. 系统产生一个「进程核心对象」，计数值为1。   
3. 系统为此进程建立一个4GB 地址空间。   
4. 加载器将必要的码加载到上述地址空间中，包括App.exe 的程序、资料，以及所需的动态联结函数库（DLLs）。加载器如何知道要加载哪些DLLs呢？它们被记录在可执行文件（PE文件格式）的.idata section 中。    
5. 系统为此进程建立一个执行线程，称为主执行线程（primary thread）。执行线程才是CPU时间的分配对象。   
6. 系统调用C runtime 函数库的Startup code。   
7. Startup code 调用App 程序的WinMain 函数。   
8. App 程序开始运作。   
9. 使用者关闭App 主窗口，使WinMain 中的消息循环结束掉，于是WinMain 结束。  
10. 回到Startup code。   
11. 回到系统，系统调用ExitProcess 结束进程。     
可以说，透过这种方式执行起来的所有Windows 程序，都是shell 的子进程。    
本来，母进程与子进程之间可以有某些关系存在，但shell在调用CreateProcess时已经把母子之间的脐带关系剪断了，因此它们事实上是独立实例。    
### 产生子进程
可以通过一个函数来专门激活其他的程序，只是这个函数的参数较为复杂。   
```c
CreateProcess(   
LPCSTR lpApplicationName,   
LPSTR lpCommandLine,   
LPSECURITY_ATTRIBUTES lpProcessAttributes,   
LPSECURITY_ATTRIBUTES lpThreadAttributes,   
BOOL bInheritHandles,   
DWORD dwCreationFlags,   
LPVOID lpEnvironment,    
LPCSTR lpCurrentDirectory,   
LPSTARTUPINFO lpStartupInfo,    
LPPROCESS_INFORMATION lpProcessInformation   
);    
```      
**第一个参数**lpApplicationName指定可执行档名称。   
**第二个参数**lpCommandLine指定欲传给新进程的命令列（command line）参数。   
> 如果你指定了lpApplicationName，但没有扩展名，系统并不会主动为你加上.EXE 扩展名；   
> 如果没有指定完整路径，系统就只在目前工作目录中寻找。但如果你指定lpApplicationName为NULL的话，系统会以lpCommandLine的第一个「段落」做为可执行档档名；   
> 如果这个档名没有指定扩展名，就采用预设的".EXE" 扩展名；   
> 如果没有指定路径，Windows 就依照五个搜寻路径来寻找可执行文件，分别是：   
1. 调用者的可执行文件所在目录   
2. 调用者的目前工作目录   
3. Windows 目录   
4. Windows System 目录   
5. 环境变量中的path 所设定的各目录   
  
```c
//系统将执行E:\CWIN95\NOTEPAD.EXE，命令列参数是"README.TXT"。    
CreateProcess("E:\\CWIN95\\NOTEPAD.EXE", "README.TXT",...);    
//系统将依照搜寻次序，将第一个被找到的NOTEPAD.EXE执行起来，并转送命令列参数"README.TXT" 给它。    
CreateProcess(NULL, "NOTEPAD README.TXT",...);    
```  
建立新进程之前，系统必须做出两个核心对象，也就是**「进程对象」和「执行线程对象」**。CreateProcess 的**第三个参数和第四个参数**分别指定这两个核心对象的安全属性。    
**第五个参数**（TRUE 或FALSE）则用来设定这些安全属性是否要被继承。   
**第六个参数**dwCreationFlags 可以是许多常数的组合，会影响到进程的建立过程。这些常数中比较常用的是CREATE_SUSPENDED，它会使得子进程产生之后，其主执行线程立刻被暂停执行。   
**第七个参数**lpEnvironment可以指定进程所使用的环境变量区。通常我们会让子进程继承父进程的环境变量，那么这里要指定NULL。    
**第八个参数**lpCurrentDirectory用来设定子进程的工作目录与工作磁盘。如果指定NULL，子进程就会使用父进程的工作目录与工作磁盘。   
**第九个参数**lpStartupInfo 是一个指向STARTUPINFO 结构的指针。这是一个庞大的结构，可以用来设定窗口的标题、位置与大小，详情请看API 使用手册。    
**最后一个参数**是一个指向PROCESS_INFORMATION 结构的指针：   
```c
typedef struct _PROCESS_INFORMATION {   
HANDLE hProcess;   
HANDLE hThread;   
DWORD dwProcessId;   
DWORD dwThreadId;   
} PROCESS_INFORMATION;   
```    
当系统为我们产生**「进程对象」和「执行线程对象」**，它会把两个对象的句柄（handle）填入此结构的相关字段中，应用程序可以从这里获得这些句柄。      
如果一个进程想结束自己的生命，只要调用：   
```c
VOID ExitProcess(UINT fuExitCode);   
```   
就可以了。如果进程想结束另一个进程的生命，可以使用：    
```c
BOOL TerminateProcess(HANDLE hProcess, UINT fuExitCode);   
```    
我们完全可以通过TerminateProcess来结束一个进程的生命。但是这个函数并不建议被使用，因为一般进程结束时，系统会通知该进程所开启（所使用）的所有DLLs，但如果你以TerminateProcess 结束一个进程，系统不会做这件事。     
之前说过：母进程与子进程之间可以有某些关系存在，但shell在调用CreateProcess时已经把母子之间的脐带关系剪断了，因此它们事实上是独立实例。   
只要通过函数CloseHandle把子进程（的句柄）关闭，就能达到这个目的。   
```c   
PROCESS_INFORMATION ProcInfo;   
BOOL fSuccess;    
fSuccess = CreateProcess(...,&ProcInfo);    
if (fSuccess)     
{   
	CloseHandle(ProcInfo.hThread);   
	CloseHandle(ProcInfo.hProcess);   
}    
```    
### 一个执行线程的诞生与死亡
程序代码的执行，是执行线程的工作。当一个进程建立起来，主执行线程也产生。所以每一个Windows程序一开始就有了一个执行线程。   
我们可以调用CreateThread 产生额外的执行线程，系统会帮我们完成下列事情：   
1. 配置「执行线程对象」，其handle 将成为CreateThread 的传回值。   
2. 设定计数值为1。   
3. 配置执行线程的context。   
4. 保留执行线程的堆栈。   
5. 将context 中的堆栈指针缓存器（SS）和指令指针缓存器（IP）设定妥当。   
程序若欲产生一个新执行线程，调用CreateThread 即可办到：   
```c
CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,   
DWORD dwStackSize,   
LPTHREAD_START_ROUTINE lpStartAddress,   
LPVOID lpParameter,   
DWORD dwCreationFlags,    
LPDWORD lpThreadId   
);    
```
第一个参数表示安全属性的设定以及继承，请参考API 手册。Windows 95 忽略此一参数。    
第二个参数设定堆栈的大小。   
第三个参数设定「执行线程函数」名称，而该函数的参数则在这里的第四个参数设定。   
第五个参数如果是0，表示让执行线程立刻开始执行，如果是CREATE_SUSPENDED ，则是要求执行线程暂停执行（那么我们必须调用
ResumeThread才能令其重新开始）。   
最后一个参数是个指向DWORD 的指针，系统会把执行线程的ID 放在这里。   
以下为一个实例：  
```c
VOID ReadTime(VOID)   
{   
	char str[50];   
	SYSTEMTIME st;   
	while(1)  
	{   
		GetSystemTime(&st);    
		sprintf(str,"%u:%u:%u", st.wHour, st.wMinute, st.wSecond);   
		SetDlgItemText (_hWndDlg, IDE_TIMER, str);   
		Sleep (1000); // 延迟一秒。   
	}   
}   

int main()   
{    
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadTime, NULL, 0, &ThreadID);   
}    
```   