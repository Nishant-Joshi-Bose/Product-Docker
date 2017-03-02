#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

static struct pcm_config pcmConfig = {
  .period_size = 1024,
  .period_count = 2,
  .channels = 2,
  .rate=48000,
  .format=PCM_FORMAT_S16_LE,
  .silence_threshold = 2048,
  .start_threshold = 1024,
};

static struct pcm *pcm;
//static char *pcmBuffer = NULL;
size_t pcmBufferSize = -1;
#define MAX_NUM_SOUNDFILES 10 // We only do up to 10 sounds for now; change if we need more
static char *fileBuffers[MAX_NUM_SOUNDFILES];
static size_t fileBufferSizes[MAX_NUM_SOUNDFILES];
static int numFiles = 0;

// Yeah, we pretty much never free up memory, etc.  Mostly coz the only time we ever would is if the program is killed
int initSound(void)
{
  pcm = pcm_open(0, 0, PCM_OUT, &pcmConfig);
  if (pcm == NULL) {
    fprintf(stderr, "failed to allocate memory for pcm\n");
    return -1;
  } else if (!pcm_is_ready(pcm)) {
    fprintf(stderr, "failed to open for pcm 0,0\n");
    pcm_close(pcm);
    return -1;
  }
  pcmBufferSize = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
  printf("pcm buffer size is %d\n", pcmBufferSize);
  /*
  pcmBuffer = malloc(pcmBufferSize);
  if (pcmBuffer == NULL) {
    fprintf(stderr, "can't allocate pcm buffer");
    pcm_close(pcm);
    return -1;
  }
  */

  return 0;
}

int addSoundFile(char *file)
{
  printf("adding sound file %s\n", file);
  struct stat statbuf;
  if (stat(file, &statbuf) < 0) {
    fprintf(stderr, "can't stat sound file %s, error %d\n", file, errno);
    return -1;
  }
  if ((fileBuffers[numFiles] = malloc(statbuf.st_size)) == NULL) {
    fprintf(stderr, "can't alloc memory for file %s\n", file);
    return -1;
  }
  FILE *f = fopen(file, "rb");
  if (f == NULL) {
    fprintf(stderr, "can't open sound file %s, error %d\n", file, errno);
    free(fileBuffers[numFiles]);
    return -1;
  }
  size_t bufPos = 0;
  size_t numRead = 0;
  while ((numRead = fread(&fileBuffers[numFiles][bufPos], 1, statbuf.st_size - bufPos, f)) > 0) {
    printf("Read %d bytes\n", numRead);
    if (numRead >= statbuf.st_size - bufPos) {
      break;
    }
    bufPos += numRead;
  }
  /*
  if (numRead < 0) {
    fprintf(stderr, "error reading sound file %s, %d\n", file, errno);
    free(fileBuffers[numFiles]);
    return -1;
  }
  */
  fclose(f);

  fileBufferSizes[numFiles] = statbuf.st_size;
  printf("Added file %s, size %d\n", file, fileBufferSizes[numFiles]);
  numFiles++;
  return 0;
}

#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))

int playSound(int soundNum)
{
  printf("playing sound %d\n", soundNum);
  if (soundNum >= numFiles) {
    fprintf(stderr, "No such sound file exists %d\n", soundNum);
    return -1;
  }
#ifdef TINYPLAYEXAMPLEDOESNTWORK
  size_t curPos = 0;
  while (curPos < fileBufferSizes[soundNum]) {
    size_t numBytesToWrite = MIN(fileBufferSizes[soundNum] - curPos, pcmBufferSize);
    printf("writing pcm data, curpos=%d, numbytes=%d\n", curPos, numBytesToWrite);
    if (pcm_writei(pcm, &fileBuffers[soundNum][curPos], numBytesToWrite) < 0) {
      fprintf(stderr, "error playing sample\n");
      return -1;
    }
    curPos += pcmBufferSize;
  }
#else
  if (pcm_writei(pcm, fileBuffers[soundNum], pcm_bytes_to_frames(pcm,fileBufferSizes[soundNum])) < 0) {
    fprintf(stderr, "error playing sound %d\n", soundNum);
    return -1;
  }
#endif

  return 0;
}

