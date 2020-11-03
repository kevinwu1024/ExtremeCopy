
// 定义回调函数

// 开始一个文件复制
typedef int (fnOnXCBeginCopyFileA*)(LPCSTR lpszSrcFile,LPCSTR lpszDstFile) ; // ansci 
typedef int (fnOnXCBeginCopyFileW*)(LPWSTR lpszSrcFile,LPWSTR lpszDstFile) ; // unicode

typedef int (fnOnXCOneFileCompleted*)() ;
typedef int (fnOnXCCopyError*)(ECopyFileErrorCode nErrorCode,int nSystemErrorCode,int SupportType) ;
typedef int (fnOnXCCopyDataSizeOccured*)(unsigned int nSize) ;
//typedef int (fnOnXCCopyPhaseDone*)() ;

typedef int (fnOnXCSameFileNameA*)(LPCSTR lpszSrcFile,LPCSTR lpszDstFile) ; // ansci
typedef int (fnOnXCSameFileNameW*)(LPWSTR lpszSrcFile,LPWSTR lpszDstFile) ; // unicode



extern "C"
{
	int __stdcall AttachFileA(LPCSTR lpFile) ;
}