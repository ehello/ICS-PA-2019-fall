#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  __off_t open_offset;//类型？
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_FBSYNC, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},//加上文件偏移量，初值为0
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read,fb_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read,fbsync_write},
  {"/proc/dispinfo",128,0,0,dispinfo_read,invalid_write},//是dispinfo字符数组的大小
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = screen_width()*screen_height()*4;//00RRGGBB
}

int fs_open(const char *pathname, int flags, int mode){
  assert(pathname != NULL);
  int fd = 0;
  for (;fd<NR_FILES;fd++){
    if(strcmp(file_table[fd].name,pathname) == 0){
      file_table[fd].open_offset = 0;
      return fd;
    }
  }
  panic("shoul not reach here");
  return -1;
}

size_t fs_offset(int fd){//并没有要求实现这个，但是如果不实现，不知道在loader里面如何把节加载到正确位置
  return file_table[fd].disk_offset;
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

__ssize_t fs_read(int fd, void *buf, size_t len){//返回值类型？
  __ssize_t ret = 0;
  switch(fd){
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR: 
    case FD_FB:
      break;
    case FD_EVENTS: ret = file_table[fd].read(buf,file_table[fd].open_offset,len);break;
    case FD_FBSYNC: break;
    case FD_DISPINFO:{
      ret = file_table[fd].read(buf,file_table[fd].open_offset,len);
    }break;

    default:{
      if(file_table[fd].open_offset >= file_table[fd].size) 
        return 0;
      if(file_table[fd].open_offset+len >file_table[fd].size) 
        len = file_table[fd].size - file_table[fd].open_offset;
      ret = ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
      file_table[fd].open_offset += ret;
    }break;
  }
  return ret;
  /*int ret = 0;
	//Log("fs_read: fd = %d, name = %s, offset = %d, len = %d", fd, file_table[fd].name, file_table[fd].open_offset, len);
	if(fd != FD_EVENTS && file_table[fd].open_offset + len > file_table[fd].size){
		len = file_table[fd].size - file_table[fd].open_offset;
    }

	if(file_table[fd].read == NULL){
  	ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	}
	else ret = file_table[fd].read(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += ret;
    //Log("success!");
return ret;*/
}

extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

__ssize_t fs_write(int fd, const void *buf, size_t len){
  switch(fd){
    case FD_STDIN: break;
    case FD_STDOUT:
    case FD_STDERR: file_table[fd].write(buf,0,len);break;
    case FD_FB:{
      if(file_table[fd].open_offset >= file_table[fd].size)
        return 0;
      if(file_table[fd].open_offset + len > file_table[fd].size)
        len = file_table[fd].size - file_table[fd].open_offset;
      file_table[fd].write(buf, file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
    }break;
    case FD_EVENTS:
    case FD_FBSYNC: file_table[fd].write(buf,0,len);break;
    case FD_DISPINFO: break;

    default:{
      if(file_table[fd].open_offset >= file_table[fd].size) 
        return 0;
      if(file_table[fd].open_offset+len > file_table[fd].size)
        len = file_table[fd].size - file_table[fd].open_offset;
      ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
    }break;
    
  }
  return len;
}
  

__off_t fs_lseek(int fd, __off_t offset, int whence){
  switch(whence){
    case SEEK_SET:{
      if(offset>=0&&offset<=file_table[fd].size){
        file_table[fd].open_offset = offset;
      }
    }break;
    case SEEK_CUR:{
      if((offset+file_table[fd].open_offset >= 0)
          &&(offset+file_table[fd].open_offset<=file_table[fd].size)){
        file_table[fd].open_offset += offset;
      }
    }break;
    case SEEK_END:{
      file_table[fd].open_offset =file_table[fd].size+offset; 
    }break;
  }
  if(file_table[fd].open_offset > file_table[fd].size)
    file_table[fd].open_offset = file_table[fd].size;
  return file_table[fd].open_offset;
}

int fs_close(int fd){
  return 0;
}
