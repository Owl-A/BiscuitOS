#include "streamer.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include <stdbool.h>

struct disk_stream* diskstreamer_new(int disk_id){
  struct disk* disk = disk_get(disk_id);
  if(!disk){
    return 0;
  }

  struct disk_stream* stream = kzalloc(sizeof(struct disk_stream));
  stream->pos = 0;
  stream->disk = disk;
  return stream;
}

int diskstreamer_seek(struct disk_stream* stream, int pos){
  stream->pos = pos;
  return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total){
  int sector = stream->pos / SECTOR_SIZE;
  int offset = stream->pos % SECTOR_SIZE;
  char buf[SECTOR_SIZE];

  int res = disk_read_block(stream->disk, sector, 1, buf);
  if(res < 0){
    goto out;
  }

  int total_to_read = ((offset + total > SECTOR_SIZE) ? SECTOR_SIZE - offset  : total);
  for ( int i = 0; i < total_to_read; i++){
    *(char*)out++ = buf[offset + i];
  }

  stream->pos += total_to_read;
  total -= total_to_read;
  if (total > 0){
    res = diskstreamer_read(stream, out, total);
  }

out:
  return res;
}

void diskstreamer_close(struct disk_stream* stream){
  kfree(stream);
}

