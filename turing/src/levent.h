
#ifndef _LEVENT_H_
#define _LEVENT_H_

/* Input : Asynchronous Input */

#define EventMode_InputToken		0
#define EventMode_InputLine		1
#define EventMode_InputCount		2
#define EventMode_InputUnblocked	3
#define EventMode_Event			4
#define EventMode_Delay			5
#define EventMode_MouseButton		6
#define EventMode_PlayNoteDone		7
#define EventMode_PlayFreqDone		8
#define EventMode_PlayFileDone		9
#define EventMode_Viewer		10
#define EventMode_NetConnect		11
#define EventMode_NetAccept		12
#define EventMode_PicDrawFrames		13
#define EventMode_PicDrawSpecial	14
#define EventMode_DialogClose		15

typedef struct EventDescriptor {
    char	mode;
    long	count;
    void	*data;
} EventDescriptor;

#endif /* #ifndef _LEVENT_H_ */


