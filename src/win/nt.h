#ifndef FS_NT_H
#define FS_NT_H

#ifndef FILE_OVERWRITE_IF
#define FILE_OVERWRITE_IF 0x00000005
#endif

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _FILE_STANDARD_INFORMATION {
  LARGE_INTEGER AllocationSize;
  LARGE_INTEGER EndOfFile;
  ULONG NumberOfLinks;
  BOOLEAN DeletePending;
  BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

typedef enum _FILE_INFORMATION_CLASS {
  FileDirectoryInformation = 1,
  FileFullDirectoryInformation,                 // 2
  FileBothDirectoryInformation,                 // 3
  FileBasicInformation,                         // 4
  FileStandardInformation,                      // 5
  FileInternalInformation,                      // 6
  FileEaInformation,                            // 7
  FileAccessInformation,                        // 8
  FileNameInformation,                          // 9
  FileRenameInformation,                        // 10
  FileLinkInformation,                          // 11
  FileNamesInformation,                         // 12
  FileDispositionInformation,                   // 13
  FilePositionInformation,                      // 14
  FileFullEaInformation,                        // 15
  FileModeInformation,                          // 16
  FileAlignmentInformation,                     // 17
  FileAllInformation,                           // 18
  FileAllocationInformation,                    // 19
  FileEndOfFileInformation,                     // 20
  FileAlternateNameInformation,                 // 21
  FileStreamInformation,                        // 22
  FilePipeInformation,                          // 23
  FilePipeLocalInformation,                     // 24
  FilePipeRemoteInformation,                    // 25
  FileMailslotQueryInformation,                 // 26
  FileMailslotSetInformation,                   // 27
  FileCompressionInformation,                   // 28
  FileObjectIdInformation,                      // 29
  FileCompletionInformation,                    // 30
  FileMoveClusterInformation,                   // 31
  FileQuotaInformation,                         // 32
  FileReparsePointInformation,                  // 33
  FileNetworkOpenInformation,                   // 34
  FileAttributeTagInformation,                  // 35
  FileTrackingInformation,                      // 36
  FileIdBothDirectoryInformation,               // 37
  FileIdFullDirectoryInformation,               // 38
  FileValidDataLengthInformation,               // 39
  FileShortNameInformation,                     // 40
  FileIoCompletionNotificationInformation,      // 41
  FileIoStatusBlockRangeInformation,            // 42
  FileIoPriorityHintInformation,                // 43
  FileSfioReserveInformation,                   // 44
  FileSfioVolumeInformation,                    // 45
  FileHardLinkInformation,                      // 46
  FileProcessIdsUsingFileInformation,           // 47
  FileNormalizedNameInformation,                // 48
  FileNetworkPhysicalNameInformation,           // 49
  FileIdGlobalTxDirectoryInformation,           // 50
  FileIsRemoteDeviceInformation,                // 51
  FileUnusedInformation,                        // 52
  FileNumaNodeInformation,                      // 53
  FileStandardLinkInformation,                  // 54
  FileRemoteProtocolInformation,                // 55
  FileRenameInformationBypassAccessCheck,       // 56
  FileLinkInformationBypassAccessCheck,         // 57
  FileVolumeNameInformation,                    // 58
  FileIdInformation,                            // 59
  FileIdExtdDirectoryInformation,               // 60
  FileReplaceCompletionInformation,             // 61
  FileHardLinkFullIdInformation,                // 62
  FileIdExtdBothDirectoryInformation,           // 63
  FileDispositionInformationEx,                 // 64
  FileRenameInformationEx,                      // 65
  FileRenameInformationExBypassAccessCheck,     // 66
  FileDesiredStorageClassInformation,           // 67
  FileStatInformation,                          // 68
  FileMemoryPartitionInformation,               // 69
  FileStatLxInformation,                        // 70
  FileCaseSensitiveInformation,                 // 71
  FileLinkInformationEx,                        // 72
  FileLinkInformationExBypassAccessCheck,       // 73
  FileStorageReserveIdInformation,              // 74
  FileCaseSensitiveInformationForceAccessCheck, // 75
  FileKnownFolderInformation,                   // 76
  FileMaximumInformation
} FILE_INFORMATION_CLASS,
  *PFILE_INFORMATION_CLASS;

NTSYSCALLAPI NTSTATUS
NtClose (
  HANDLE Handle
);

NTSYSCALLAPI NTSTATUS
NtOpenFile (
  PHANDLE FileHandle,
  ACCESS_MASK DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK IoStatusBlock,
  ULONG ShareAccess,
  ULONG OpenOptions
);

NTSYSCALLAPI NTSTATUS
NtCreateFile (
  PHANDLE FileHandle,
  ACCESS_MASK DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK IoStatusBlock,
  PLARGE_INTEGER AllocationSize,
  ULONG FileAttributes,
  ULONG ShareAccess,
  ULONG CreateDisposition,
  ULONG CreateOptions,
  PVOID EaBuffer,
  ULONG EaLength
);

NTSYSCALLAPI NTSTATUS
NtReadFile (
  HANDLE FileHandle,
  HANDLE Event,
  PVOID ApcRoutine,
  PVOID ApcContext,
  PIO_STATUS_BLOCK IoStatusBlock,
  PVOID Buffer,
  ULONG Length,
  PLARGE_INTEGER ByteOffset,
  PULONG Key
);

NTSYSCALLAPI NTSTATUS
NtWriteFile (
  HANDLE FileHandle,
  HANDLE Event,
  PVOID ApcRoutine,
  PVOID ApcContext,
  PIO_STATUS_BLOCK IoStatusBlock,
  PVOID Buffer,
  ULONG Length,
  PLARGE_INTEGER ByteOffset,
  PULONG Key
);

NTSYSCALLAPI NTSTATUS
NtQueryInformationFile (
  HANDLE FileHandle,
  PIO_STATUS_BLOCK IoStatusBlock,
  PVOID FileInformation,
  ULONG Length,
  FILE_INFORMATION_CLASS FileInformationClass
);

#endif // FS_NT_H
