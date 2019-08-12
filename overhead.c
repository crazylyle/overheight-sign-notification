/* 
   overhead.c -- a program to signal when an overheight sign
   goes off.

   Written, 2018,  by James Peterson, Austin, Texas
   At the request of Clark "Byrd" Thomas, 
   Austin Traffic Signals, Plugerville, Texas.

   All rights reserved.
*/
   

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
/* This is a program written for the Texas Department of
   Transportation (TxDOT) to notify them of "Overheight Events".
   An Overheight Event is when a truck approaches an overhead
   bridge which is lower than the truck.  If the truck keeps
   going, it will hit the bridge, causing damage to the truck and
   the bridge.

   To prevent such problems, each bridge has a sign on it that
   says how tall it is.  Most bridges are at least 16 feet over
   the road.  A too-tall truck needs to exit the highway, and go
   around the bridge.  So they have to be told the bridge height
   before the bridge, and even before the last exit that will
   take them around the bridge. 

   Where there are problems, the state will install an infrared
   beam that crosses the highway at (or below) the bridge height.
   If the truck breaks the beam, it is too tall, and breaking the
   beam causes a sign to flash saying, effectively "Too Tall!
   Exit now!".  This program is part of a project to record the
   time and date of these overhead events -- the breaking of the
   beam.  We know how often the truck hits the bridge and damage
   is done :( but, if the truck breaks the beam, sees the sign
   flashing, and exits, going around the low bridge, no damage is
   done, and we don't know it happened.

   The design for this Overheight Vehicle Detection System is in
   a PDF document, CVM-VCS-Protocol-2.0.2.pdf.  This says there will
   be a Commercial Vehicle Management (CVM) server somewhere, and
   we are to report overheight events to the CVM server.  The
   document lays out how this is to be done.

   Communication is done over TCP/IP.  We assume that the IP
   addresses are being statically assigned by TxDOT.  Our code
   will create a socket and wait for CVM to connect to us.  We do
   not create a connection; only CVM does that.  They will send
   us a retrieveDataReq request message and we will respond to
   that with a retrieveDataResp response message.  The request
   message asks for our status, and we respond with the last
   overheight event data that we have.  If an overheight event
   happens, we remember it and (if the connection is still up)
   send an overheightUpdateMsg message to CVM.  At the moment,
   that's all there is to it.

   All these messages are in XML.  There are XML description
   documents available from TxDOT, but they seem to be
   effectively the following:

   <retrieveDataReq>
       <refId> aaa </refId>
       <icdVersion> bbb </icdVersion>
       <overheightData> TRUE </overheightData>
   </retrieveDataReq>

   where the refId is assigned by CVM and the icdVersion is what
   it is (probably 1.0 at this point?)

   Our response message looks then like:

    <retrieveDataResp>
    	<refId> aaa </refId>
    	<icdVersion> bbb </icdVersion>
    	<overheightData>
    		<id>
    			<providerName> fff1 </providerName>
    			<resourceType> fff2 </resourceType>
    			<centerId> fff3 </centerId>
    		</id>
    		<overheight>
    			<overheightStatus>
    				<opStatus> ggg </opStatus>
    			</overheightStatus>
    			<overheightReadingData>
    			    <readingTime> iii </readingTime>
    				<readingDate> jjj </readingDate>
    				<triggerHeight> kkk </triggerHeight>
    		    </overheightReadingData>
    		</overheight>
    	</overheightData>
    </retrieveDataResp>

    We just mimic back the refID and icdVersion.  The
    identification fields (fff1, fff2, fff3) are assigned by some
    agency, and we just report them.  The opStatus is either
    Active, Error, Failed, or OutofService.  iii is from the time
    of the event (formatted as HH:MM:SS), jjj is from the date of
    the event (formatted as YYYY-MM-DD) kkk is height which would
    be constant for a given detector.  So, in summary, we mimic
    two fields, 4 fields are constant for a given detector (but
    may vary from detector to detector), 2 fields are the actual
    data from the last overheight event, and one field is the
    status, which is pretty well always "Active" (we hope).

    An overheightUpdateMsg message is pretty well the same
    as a retrieveDataResp response message, but slightly different:

    <overheightUpdateMsg>
    	<refId> aaa </refId>
    	<icdVersion> bbb </icdVersion>
    	<id>
    		<providerName> fff1 </providerName>
    		<resourceType> fff2 </resourceType>
    		<centerId> fff3 </centerId>
    	</id>
    	<overheight>
    		<overheightStatus>
    			<opStatus> ggg </opStatus>
    		</overheightStatus>
    		<overheightReadingData>
    		    <readingTime> iii </readingTime>
    			<readingDate> jjj </readingDate>
    			<triggerHeight> kkk </triggerHeight>
    	    </overheightReadingData>
    	</overheight>
    </overheightUpdateMsg>

    Here, we (the detector) get to make up our own refId.  The
    icdVersion should be the same as what CVM sends us.  The 
    other fields are just like what we saw before.

    This code is written for a Moxa ioPAC 5542-C-T as the detector.
    This is a small box with a bunch of input/output ports.  The
    wire from the infrared beams can be put into the box and
    will cause it to generate a signal.  When we get that signal,
    we record the time and date and generate an overheightUpdateMsg.

    The Moxa runs Linux and can do network activity too, so it also
    creates the socket and responses to the messages from the CVM.

    So our program has two parts:  (a) wait for an overhead
    event, and (b) wait for a message from CVM.  We do the first
    by using some code modeled from a sample provided by Moxa.
    We do the second by code provided by TxDOT.  Oh, no, they 
    don't do that, so we had to write our own.

    As near as I can tell, the code from Moxa polls for an event;
    while our code to wait for network events is interrupt based
    (using the select() system call).  So we have two parts to
    our code: one part polls for events, the other waits for
    network activity.

    The connection between the two parts is the event data: the
    time and date of the last event.  We put this into a file in
    the file system (the event file).  The overhead event code
    writes to the event file; the network code reads from the
    event file.  

    Initially, we thought these two parts would need to be
    separate processes.  However, it looks like we need to poll
    the Moxa input lines at a frequent rate (at least every .05
    seconds), and the easiest way to do that is with a time-out
    for a select() system call.  You can sleep() for as little as
    a second, but to get sub-second waits, I can use select().
    The time-out value for a select allows the time to be
    in seconds and nanoseconds.  Since we need a select() to
    wait for the network part, we can combine the two into one
    select() which waits for either network activity or a
    time-out.  If network activity, process it.  If a time-out,
    go poll the MOXA input lines.

    The constant parts of the messages that can vary from
    detector to detector are set in a configuration file.  When
    we start up, we read the configuration file and use it to
    define all and any variables that we may need to set.  The
    configuration file is set up to be simple and extensible.  It
    can be prepared by anything that you want to use to create a
    simple text file.

    We also keep, locally, a log file that keeps track of all
    the important things that this program does.  If it does
    not seem to be working correctly, check the log file to
    see what it thinks it is doing.

    Also, for convenience in testing, we handle a number of
    signals that can be used to control the program.  For
    example, a SIGUSR1 causes the program to act as if an
    overhead event has been detected.  It should then update the
    event file, and cause an overheightUpdateMsg message to be
    generated.  

    SIGPWR will cause the program to re-read its configuration
    file.  This allows the config file to be editted to 
    change the status of a device, and then have that 
    changed value read and used.

    For simplicity of distribution, this is just one big C file.
    In many ways, it would make sense to break it into several
    parts, but that makes distributing it and compiling it just
    that much harder, so I'm doing it as one file.  
*/

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <stdio.h>        /* fopen, fprint, fclose, ... */
#include <stdlib.h>       /* malloc, free */
#include <unistd.h>       /* getpid, open, close, ... */
#include <string.h>       /* strcmp, strlen, ... */
#include <ctype.h>        /* tolower, isspace, isalpha, ... */
#include <stdarg.h>       /* va_list, va_start, va_end, ... */
#include <time.h>         /* time, localtime, ... */
#include <errno.h>        /* errno, perror, ... */
#include <strings.h>      /* strcasecmp,  ... */
#include <sys/types.h>    /* open */
#include <fcntl.h>        /* O_CREAT, O_DSYNC, ... */
#include <signal.h>       /* signal, SIGUSR1, ... */

#include "RTU/libmoxa_rtu.h"  /* struct Timestamp, MX_RTU_Module_DI_Value_Get */

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Common types and macro definitions that we use. */

typedef unsigned long UINT32;
typedef long INT32;
typedef unsigned short UINT16;
typedef short INT16;
typedef unsigned char UINT8;
typedef char INT8;

/* we know that 0 is false and 1 is true, but let's be more explicit */
typedef short Boolean;
#define TRUE 1
#define FALSE 0

typedef char *STRING;
#define STRING_EQUAL(a,b) (strcmp(a,b) == 0)

/* A file descriptor -- just an int */
typedef int FileDesc;

/* make casts more obvious */
#define CAST(t,e) ((t)(e))

/* malloc of typed objects */
#define TYPED_MALLOC(t) CAST(t*, malloc(sizeof(t)))

/* should be more than 256 */
#define MAX_FILENAME_LENGTH  320

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* MALLOC space for a string and copy it */

STRING remember_string(const STRING name)
{
    size_t n;
    STRING p;

    if (name == NULL) return(NULL);

    /* get memory to remember file name */
    n = strlen(name) + 1;
    p = CAST(STRING, malloc(n));
    strcpy(p, name);
    return(p);
}

#define UPDATE_STRING(s,v) { if (s != NULL) free(s); s = remember_string(v); }

Boolean mystrcasecmp(STRING s, STRING t)
{
    /* as long as the two strings are equal, keep going */
    while (tolower(*s) == tolower(*t))
        {
            /* if we reach the end of one string
               (and they are still equal, so also the end
               of the other string), they are equal. */
            if (*s == '\0') return(TRUE);
            s++;
            t++;
        }
    return(FALSE);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */
/* Forward declarations. */

/* important does all the logging, so it is needed everywhere.
   But it needs to check if we need a new log file, and if we
   do, we will need to dump the state, which needs to log stuff
   in the new log file. */

void important(const char *format, ...);


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* General main program variables */

#define DEFAULT_HOME_DIRECTORY "/home/overhead"
STRING Home_Directory = DEFAULT_HOME_DIRECTORY;

#define DEFAULT_LOG_DIRECTORY "/home/overhead/log"
STRING Log_Directory = DEFAULT_LOG_DIRECTORY;

#define DEFAULT_CONFIG_FILENAME    "config.txt"
STRING Config_FileName = DEFAULT_CONFIG_FILENAME;

STRING Log_FileName = NULL;
FILE *log_file = NULL;


Boolean debug = FALSE;
Boolean verbose = TRUE;

STRING today = NULL;

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Variables needed for the XML messages. */

int myRefId = 0;
STRING icdVersion = NULL;

/* Variables needed for the network connections. */

STRING PortName = NULL;
STRING StringMyRefId = NULL;

/* maximum allowed length of an XML request message */
#define MAX_MESSAGE_LENGTH  100000


/* number of microseconds between polling */
/* How do we come to this number?  Figure a vehicle traveling 75
   miles per hour.  That works out to 110 feet per second.  For a
   10-foot vehicle, that would be about 0.1 second.  But you have
   to sample at at least twice the frequency of what you want to
   see, so cut that in half to 0.05 seconds, or 50 milliseconds,
   50000 microseconds.  Let's make this configurable. */

#define POLLING_DELAY  50000
int pollingDelay = POLLING_DELAY;

#define MAX_LOG_FILE_DIRECTORY_SIZE  6000000
int Log_File_Limit = MAX_LOG_FILE_DIRECTORY_SIZE;

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Assign default, load-time, values for DI variables */

/* Slot of DI module [0-9]. Default slot = 0 */
/* The slot is normally 0, for the built-in DI channels.  It
   is supposed to be possible to have expansion modules which
   create more channels.  This will have slots 1, 2, and up.
   We don't foresee that we would need those, so we hardwire
   the slot to zero, but we keep it, in case someone needs to
   extend this code later.  That would mean any specification
   of a channel (currently 0 to 7) would have to be a pair 
   with a slot (0 to ...) and a channel within that slot. */
UINT8 diSlot = 0;

#define MAX_CHANNELS 8


/* A MOXA box has 8 input wires (channels).  Each detector device
   needs two wires:  (a) for an overheight event and (b) for a 
   detected device fault.

   So a given MOXA box can support up to 4 detector devices. To
   be able to do that, we pre-allocate a table of 4 detector
   device descriptors (DDD[4]) most of which will be empty.
*/
#define MAX_DETECTORS 4

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The protocol says that there are 4 states for the detectors
   to be in.  We have to report status. */
enum DeviceStatus { ST_ACTIVE, ST_ERROR, ST_FAILED, ST_OUTOFSERVICE };


STRING Format_Device_Status(enum DeviceStatus status)
{
    switch (status)
        {
        case ST_ACTIVE:  return("Active");
        case ST_ERROR:   return("Error");
        case ST_FAILED:  return("Failed");
        case ST_OUTOFSERVICE:  return("OutofService");
        }
    return("unknown status");
}

enum DeviceStatus decode_status(STRING value)
{
    if (value != NULL)
        {
            if (mystrcasecmp(value, "Active"))
                {
                    return(ST_ACTIVE);                    
                }
            if (mystrcasecmp(value, "OutofService"))
                {
                    return(ST_OUTOFSERVICE);
                }
            if (mystrcasecmp(value, "Failed"))
                {
                    return(ST_FAILED);
                }
        }
    return(ST_ERROR);
}

int decode_channel_number(STRING value)
{
    int n = atoi(value);
    if (n < 0) n = 0;
    if (n >= MAX_CHANNELS) n = MAX_CHANNELS-1;
    return(n);
}

int decode_polling_delay(STRING value)
{
    /* polling delay is from 1 to 1000000 microseconds */
    int n = atoi(value);
    if (n < 10) n = 0;
    if (n >= 1000000) n = 1000000;
    return(n);
}


int decode_file_size(STRING value)
{
    /* a file size can be a number <n> or <n>K or <n>M */
    int n = 0;
    while (isdigit(*value))
        {
            n = n*10 + (*value - '0');
            value++;
        }
    if ((*value == 'K') || (*value == 'k'))
        n = n * 1024;
    if ((*value == 'M') || (*value == 'm'))
        n = n * 1024 * 1024;
    return(n);
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* information needed for each detector -- can have up to 4 */
struct detector_device_descriptor
{
    /* the following fields are unchanging once they are defined */
    /* used to identify the detector for the config file */
    STRING name;

    /* values needed to send back to CVM in the XML */
    STRING providerName;
    STRING resourceType;
    STRING centerId;
    STRING id;
    STRING triggerHeight;

    /* the two channels, one for the event; one for the fault */
    int event_channel;
    int fault_channel;    

    /* used interally to store the last event time/date */
    STRING eventFileName;

    /* the following fields may actually change */
    enum DeviceStatus status;
    
};

typedef struct detector_device_descriptor *DEVICE;


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

DEVICE DDD[MAX_DETECTORS] = { NULL };

/* we define a default device to hold any key/value
   pairs that should have a device, but do not. */
struct detector_device_descriptor default_device =
{
    /*  name, providerName, resourceType, centerId, id, triggerHeight, */
        NULL,         NULL,         NULL,     NULL, NULL,        NULL, 
    /* event_channel, fault_channel, filename, status  */
                  -1,            -1,     NULL, ST_ERROR
};



/* For each channel -- there can be 8 of them -- we want to know
   what detector device is attached to it.  We use a simple
   array, which points at the detector device descriptor. 
   We need to set up this array when we set up the detector
   device descriptor table, at config time.  If a Channel_Table
   entry is not NULL, then it points to a detector device 
   descriptor which has either an event channel or fault channel
   set to that channel.
*/
DEVICE Channel_Table[MAX_CHANNELS] = {NULL};


DEVICE search_device_array(STRING name)
{
    int i;
    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            if (d == NULL) continue;
            if (mystrcasecmp(name, d->name))
                return(d);
        }

    /* didn't find it; make a new entry */
    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            if (d != NULL) continue;

            d = DDD[i] = TYPED_MALLOC(struct detector_device_descriptor);
            d->name = remember_string(name);
            d->providerName = NULL;
            d->resourceType = NULL;
            d->centerId = NULL;
            d->id = NULL;            
            d->triggerHeight = NULL;
            d->event_channel = -1;
            d->fault_channel = -1;    
            d->eventFileName = NULL;
            d->status = ST_ERROR;
            if (debug) fprintf(stderr, "new device %d: %s\n", i, d->name);
            return(d);
        }
    
    important("too many devices -- maximum of %d\n", MAX_DETECTORS);
    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            important("%d: %s\n", i, d->name);
        }
    
    return(NULL);
}

void free_DDD(DEVICE d)
{
    if (d->name != NULL) free(d->name);
	if (d->providerName != NULL) free(d->providerName);
	if (d->resourceType != NULL) free(d->resourceType);
	if (d->centerId != NULL) free(d->centerId);
	if (d->id != NULL) free(d->id);
	if (d->triggerHeight != NULL) free(d->triggerHeight);
	if (d->eventFileName != NULL) free(d->eventFileName);
    free(d);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Dump_Program_State(void)
{
    int i;
    
    important("Initial Configuration Values\n");
    important("Listen on port %s\n", PortName);
    important("Our RefId starts at %s\n", StringMyRefId);
    important("Our icdVersion is %s\n", icdVersion);
    important("Polling delay is %d microseconds\n", pollingDelay);
    important("Log File Limit is %d bytes\n", Log_File_Limit);

    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            if (d == NULL) continue;

            important("\n");
            important("Detector Device Config Name: %s\n", d->name);
            important("\t centerId: %s\n", d->centerId);            
            important("\t providerName: %s\n", d->providerName);            
            important("\t resourceType: %s\n", d->resourceType);
            important("\t id: %s\n", d->id);
            important("\t triggerHeight: %s\n", d->triggerHeight);
            important("\t status: %s\n", Format_Device_Status(d->status));
            important("\t event File Name: %s\n", d->eventFileName);
            important("\t event channel: %d\n", d->event_channel);
            important("\t fault channel: %d\n", d->fault_channel);            
        }

    important("\n");
    important("Channel Table:\n");
    for (i = 0; i < MAX_CHANNELS; i++)
        {
            DEVICE d = Channel_Table[i];
            if (d == NULL) continue;
            important("%d: %s (%s)\n", i, d->name,
                      ((d->event_channel == i) ? "event" :
                       (d->fault_channel == i) ? "fault" : "invalid")                     
                      );
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int size_of_file(STRING name)
{
    struct stat statusbuffer;

    /* stat the file, and return the size */
    if (stat(name, &statusbuffer) < 0)
        {
            perror(name);
            return(0);
        }
    return(statusbuffer.st_size);
}

void DeleteOldestFilesUntilUnderLimit(int n)
{
    /* Run thru the log file.  Find the oldest
       file and delete it. */
    while (n > Log_File_Limit)
        {
            DIR * dirp;
            struct dirent *dp;
            
            int oldest = 0;
            char oldest_name[MAX_FILENAME_LENGTH];
            int oldest_n = 0;
            
            dirp = opendir(Log_Directory);
            if (dirp == NULL)
                {
                    fprintf(stderr, "%s is unreadable\n", Log_Directory);
                    return;
                }
            
            while ((dp = readdir(dirp)) != 0)
                {
                    if (dp->d_ino == 0)
                        continue;
                    if (dp->d_name[0] == '.')
                        continue;

                    char full_name[MAX_FILENAME_LENGTH];
                    snprintf(full_name, sizeof(full_name), "%s/%s",
                             Log_Directory, dp->d_name);
                    struct stat statbuf;
                    int rc = stat(full_name, &statbuf);
                    if (rc != 0) { oldest = 0; break; }
                    
                    if ((oldest == 0) || (oldest > statbuf.st_ctime))
                        {
                            oldest = statbuf.st_ctime;
                            strncpy(oldest_name, full_name, sizeof(oldest_name));
                            oldest_n = statbuf.st_size;
                        }
                }
            closedir(dirp);

            /* make sure we found a file */
            if (oldest == 0) return;

            /* check if we are down to just the most recent file */
            if (STRING_EQUAL(oldest_name, Log_FileName)) return;
            
            /* delete the oldest file */
            int rc = unlink(oldest_name);
            important("remove %s -> %d\n", oldest_name, rc);
            n = n - oldest_n;
        }
}

void CheckForLogDirectoryFull(void)
{
    /* We have a log file directory, and in that directory */
    /* We run thru the log file directory, summing the size
       of all the files, and then check if it exceeds our
       limit */
    /* if we exceed our limit, we have to delete the oldest
       file.  Since we think we will seldom exceed our limit,
       we run thru first to see if that is the case, and
       only if we are exceeding our limit do we look for
       the oldest file, and delet it. */

    DIR * dirp;
    struct dirent *dp;

    int n = 0;

    dirp = opendir(Log_Directory);
    if (dirp == NULL)
        {
            fprintf(stderr, "%s is unreadable\n", Log_Directory);
            return;
        }

    while ((dp = readdir(dirp)) != 0)
        {
            if (dp->d_ino == 0)
                continue;
            if (dp->d_name[0] == '.')
                continue;

            char full_name[MAX_FILENAME_LENGTH];
            snprintf(full_name, sizeof(full_name), "%s/%s", Log_Directory, dp->d_name);
            n += size_of_file(full_name);
        }
    closedir(dirp);

    if (n > Log_File_Limit)
        {
            DeleteOldestFilesUntilUnderLimit(n);
        }
}


void Check_If_Need_New_Log_File(void)
{
    /* get the current date */
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    static char sdate[16];
    snprintf(sdate, sizeof(sdate), "%04d%02d%02d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

    /* check if it is still today */
    if ((today != NULL) && STRING_EQUAL(today, sdate))
        return;

    /* it is no longer today, so need to start a new log file
       for the new day */
    UPDATE_STRING(today, sdate);

    /* if we had a previous log file, close it */
    if (log_file != NULL) fclose(log_file);

    static char slogname[MAX_FILENAME_LENGTH];
    snprintf(slogname, sizeof(slogname), "%s/%s.txt", Log_Directory, sdate);
    
    log_file = fopen(slogname, "a");
    if (log_file != NULL) UPDATE_STRING(Log_FileName, slogname);

    /* start the log out with a timestamp and pid */
    important("Process ID (pid) is %d\n", getpid());
    Dump_Program_State();

    /* check if the log directory exceeds the maximum limit we set */
    CheckForLogDirectoryFull();
}

void Setup_for_Logging(void)
{
    /* important() will check by itself to make sure that we
       have a log file, and create one if need be, or append 
       onto the end of an existing one. */
    /* so the main thing we should do is make sure that the
       Log directory exists, and create it if not.  Also then
       we can check to see that it is not too big. */
    struct stat statbuf;
    int rc = stat(Log_Directory, &statbuf);
    if (errno != 0)
        {
            /* check if directory does not exist; try to create it */
            if (errno == ENOENT)
                {
                    fprintf(stderr, "Log directory (%s) does not exist\n", Log_Directory);
                    rc = mkdir(Log_Directory, 0744);
                    if (rc != 0)
                        {
                            fprintf(stderr, "Could not make Log directory (%s)\n", Log_Directory);
                            perror(Log_Directory);
                            exit(-1);
                        }
                    /* if we made the directory, stat it, so we can continue */
                    rc = stat(Log_Directory, &statbuf);
                    if (rc != 0)
                        {
                            fprintf(stderr, "Could not stat newly made Log directory (%s)\n", Log_Directory);
                            perror(Log_Directory);
                            exit(-1);
                        }
                }
        }

    /* Ok, we have stat-ed the directory.  Make sure
       it is a directory. */
    if (!S_ISDIR(statbuf.st_mode))
        {
            fprintf(stderr, "Log directory (%s) is not a directory?\n", Log_Directory);
            exit(-1);
        }

    /* check if the log directory exceeds the maximum limit we set */
    Check_If_Need_New_Log_File();
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

STRING TimeStamp(void)
{
    /* get the current date and time */
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    static char stime[32];
    snprintf(stime, sizeof(stime), "%04d-%02d-%02d %02d:%02d:%02d",
              tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec);
    return(stime);
}

/* log all important events */

void important(const char *format, ...)
{
    va_list args;

    /* if we have debugging on, write all important events to the console */
    if (debug || (log_file == NULL))
        {
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);    
        }
    
    /* log all important events to our device log file */
    if (log_file != NULL)
        {
            Check_If_Need_New_Log_File();
            fprintf(log_file, "%s: ", TimeStamp());
            va_start(args, format);
            vfprintf(log_file, format, args);
            fflush(log_file);
            va_end(args);    
        }
    
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

Boolean istagchar(int c)
{
    /* tag characters can be alphabetic, numeric, period, hyphen,
       underscore, or colon. */
    if (isalnum(c)) return(TRUE);
    if (c == '.') return(TRUE);
    if (c == '-') return(TRUE);
    if (c == '_') return(TRUE);
    if (c == ':') return(TRUE);
    return(FALSE);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Scan_Command_Line_Arguments(int argc, char **const argv)
{
    int rc;
    
	while (-1 != (rc = getopt(argc, argv, "dvqhD:L:c:l:")))
        {
            switch(rc)
                {

                case 'd': /* debug option */
                    debug = !debug;
                    break;
                    
                case 'v': /* verbose option */
                    verbose = TRUE;
                    break;
                    
                case 'q': /* verbose option */
                    verbose = FALSE;
                    break;
                    
                case 'D':
                    Home_Directory = remember_string(optarg);
                    break;

                case 'c':
                    Config_FileName = remember_string(optarg);
                    break;

                case 'L':
                    Log_Directory = remember_string(optarg);
                    break;

                case 'h':
                default:
                    printf("Overhead Detection and Reporting.\n\n");

                    printf("Usage: ./overhead [OPTIONS]\n\n");

                    printf("Options:\n");

                    printf("\t%-8s Home Directory name. Default name = %s\n",
                           "-D", Home_Directory);

                    printf("\t%-8s Configuration file name. Default name = %s\n",
                           "-c", Config_FileName);

                    printf("\t%-8s Log Directory name. Default name = %s\n",
                           "-L", Log_Directory);

                    printf("\t%-8s Verbose mode (Default = %s)\n",
                           "-v", verbose ? "TRUE" : "FALSE");


                    printf("\n");

                    exit(0);
                }
        }
}


/*******************************************************************************
 *
 * Code to read the Configuration file.  Done once when the program is started,
 * We may need some way to kick the program to cause it to re-read the config
 * file.  But for now, just kill the original program, and restart it.
 *
 ******************************************************************************/

struct config_table
{
    STRING key;
    int    string_index;
};



struct config_table cft[] =
{
    { "providerName", 0},
    { "resourceType", 1},
    { "centerId", 2},
    { "triggerHeight", 3},
    { "EventChannel", 4},
    { "FaultChannel", 5},    
    { "EventFilename", 6},
    { "opStatus", 7},
    { "PortName", 8},
    { "myRefId", 9},				   
    { "icdVersion", 10},
    { "pollingDelay", 11},
    { "id", 12},
    { "logFileLimit", 13},    
    { NULL, -1}
};



STRING line_buffer = NULL;
int line_buffer_length = 0;

STRING get_line(FILE *f)
{
    /* read a line from the file */
    int i = 0;
    int c;
    while ((c = getc(f)) != EOF)
        {
            if (i > line_buffer_length-1)
                {
                    if (line_buffer_length == 0)
                        {
                            line_buffer_length = 128;
                            line_buffer = CAST(STRING, malloc(line_buffer_length));
                        }
                    else
                        {
                            line_buffer_length = 2 * line_buffer_length;
                            line_buffer = CAST(STRING, realloc(line_buffer,line_buffer_length));
                        }
                }
            if (c == '\n') break;
            line_buffer[i] = c;
            i += 1;
        }
    
    line_buffer[i] = '\0';

    if ((c == EOF) && (i == 0)) return(NULL);
    
    return(line_buffer);
}

void define_key_string(STRING device, int string_index, STRING value)
{
    if (debug)  fprintf(stderr, "config value for %s -- %d(%s) = %s\n",
                        device, string_index, cft[string_index].key, value);

    DEVICE d = NULL;
    if (device != NULL)
        {
            d = search_device_array(device);
            if (d == NULL)
                return;
        }
    else
        {
            d = &default_device;
        }

    switch (string_index)
        {
        case 0: UPDATE_STRING(d->providerName, value); return;
        case 1: UPDATE_STRING(d->resourceType, value); return;
        case 2: UPDATE_STRING(d->centerId, value); return;
        case 3: UPDATE_STRING(d->triggerHeight, value); return;
        case 4: d->event_channel = decode_channel_number(value); return;
        case 5: d->fault_channel = decode_channel_number(value); return;
        case 6: UPDATE_STRING(d->eventFileName, value); return;
        case 7: d->status = decode_status(value); return;
        case 8: UPDATE_STRING(PortName, value); return;
        case 9: UPDATE_STRING(StringMyRefId, value); return;
        case 10: UPDATE_STRING(icdVersion, value); return;
        case 11: pollingDelay = decode_polling_delay(value); return;
        case 12: UPDATE_STRING(d->id, value); return;
        case 13: Log_File_Limit = decode_file_size(value); return;            
        }
}

void define_config_value(STRING key, STRING value)
{
    if (debug)  fprintf(stderr, "config value -- %s = %s\n", key, value);
    
    /* we want to define a key with a value. 
       Check first to see if the key has a structure.
       We allow a key to be a simple string, or
       a "device-name.key".  So look to see if there
       is a period/dot in the key. */
    STRING device = NULL;
    STRING p;
    for (p = key; *p != '\0'; p++)
        {
            if (*p == '.')
                {
                    /* define the device part */
                    device = key;
                    /* split the key string into two parts */
                    *p = '\0'; p++;
                    /* and keep the second part as the new key */
                    key = p;
                    break;
                }
        }
    
    int i;
    for (i = 0; (cft[i].key != NULL); i++)
        {
            /* if the keys are equal, define the value */
            if (mystrcasecmp(cft[i].key, key))
                {
                    define_key_string(device, cft[i].string_index, value);
                    break;
                }
        }
}

    
Boolean Read_Config_File(void)
{
    /* The config file needs to do three things:

       1. set values that control our overall processing
          that can be configured -- the port we use for
          TCP network activity, the base refId that we use
          for our messages, the name of the log file 
          and so on.

       2. It needs to build a detector device descriptor
          for each detector attached to the MOXA, and use
          those to define the DDD array.

       3. It needs to use the DDD array to build the 
          Channel_Table, so we can go from a channel back
          to the detector device that drives that channel
          (either as an event or a fault channel).
    */
    
    int i = 0;
    
    FILE *cfile = fopen(Config_FileName, "r");
    if (cfile == NULL)
        {
            important("No config file: %s\n", Config_FileName);
            return(FALSE);
        }

    line_buffer = NULL;
    line_buffer_length = 0;

    for (i = 0; i < 4; i++)
        {
            if (DDD[i] != NULL) free_DDD(DDD[i]);
            DDD[i] = NULL;
        }
    
    /* config file should consist of a sequence of lines.
       Each line has a keyword, then an value.  Mostly the
       values are strings.  We trim leading and trailing spaces.
       A line that starts with "#" is a comment.
    */

    STRING line;
    while ((line = get_line(cfile)) != NULL)
        {
            i = 0;
            /* skip leading blanks */
            while ((line[i] != '\0') && isspace(line[i])) i += 1;

            /* skip blank lines, and comments */
            if (!isalpha(line[i])) continue;

            /* find first letter of key */
            STRING key = &(line[i]);

            /* skip over the key; key is alphabetic or period */
            while (isalpha(line[i]) || (line[i] == '.')) i += 1;
            /* this character terminates the key */
            line[i] = '\0';
            i += 1;

            /* find the value */
            while ((line[i] != '\0') && !isalnum(line[i])) i += 1;

            /* found first character of the value */
            STRING value = &(line[i]);

            /* trim off any trailing spaces */
            int n = strlen(value);
            while ((n > 0) && isspace(value[n-1]))
                {
                    value[n-1] = '\0';
                    n -= 1;
                }

            /* now we have the key and value strings,
               use them to define the config values */
            define_config_value(key, value);
        }
    
    /* close the config file */
    fclose(cfile);
    /* and free any line buffer space */
    line_buffer_length = 0;
    if (line_buffer != NULL)
        {
            free(line_buffer);
            line_buffer = NULL;
        }

    /* make sure we have at least one device defined */
    /* I think we can just check DDD[0], since they
       are allocated from 0 up. */
    if ((DDD[0] == NULL) && (DDD[1] == NULL)
        && (DDD[2] == NULL) && (DDD[3] == NULL))
        {
            /* side-effect of the search is to create a DDD entry */
            (void) search_device_array("default device");
        }
    
    /* now initialize the Channel_Table. */
    for (i = 0; i < MAX_CHANNELS; i++)
        Channel_Table[i] = NULL;

    /* Walk thru the DDD array and make sure that
       all fields are defined.  If they were not 
       defined originally, then try to define them from
       the default device descriptor.  Then use the
       channels in the DDD array to define the 
       non-NULL entries in the Channel_Table. */
    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            if (d == NULL) continue;
            if (d->providerName == NULL)
                d->providerName = remember_string(default_device.providerName);
            if (d->resourceType == NULL)
                d->resourceType = remember_string(default_device.resourceType);
            if (d->centerId == NULL)
                d->centerId = remember_string(default_device.centerId);
            if (d->id == NULL)
                d->id = remember_string(default_device.id);
            if (d->triggerHeight == NULL)
                d->triggerHeight = remember_string(default_device.triggerHeight);
            if (d->event_channel == -1)
                d->event_channel = default_device.event_channel;
            if (d->fault_channel == -1)
                d->fault_channel = default_device.fault_channel;    
            if (d->eventFileName == NULL)
                d->eventFileName = remember_string(default_device.eventFileName);
            if (d->status == ST_ERROR)
                d->status = default_device.status;

            if (Channel_Table[d->event_channel] != NULL)
                important("both %s and %s use channel %d\n",
                          Channel_Table[d->event_channel]->name, d->name, d->event_channel);
            if (Channel_Table[d->fault_channel] != NULL)
                important("both %s and %s use channel %d\n",
                          Channel_Table[d->fault_channel]->name, d->name, d->fault_channel);
            Channel_Table[d->event_channel] = d;
            Channel_Table[d->fault_channel] = d;            
        }
    return(TRUE);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* used for both reading and writing.  Should be the same for both */
#define EVENT_FILE_FORMAT "%04lu/%02lu/%02lu %02lu:%02lu:%02lu\n"


void WriteEventToFile(DEVICE d, struct Timestamp *timedate)
{
    char szLine[32];

    /* This line is actually only 21 bytes long, so 32 is plenty of space */
    snprintf(szLine, sizeof(szLine), EVENT_FILE_FORMAT,
            timedate->year, timedate->mon, timedate->day,
            timedate->hour, timedate->min, timedate->sec);
    if (debug)
        fprintf(stderr, szLine);

    /* szLine has newline (/n) at the end, so we don't need another */
    important("Event for %s at: %s", d->name, szLine);
    
    /* open the output event file for write */
    FileDesc fd = open(d->eventFileName, O_WRONLY|O_CREAT|O_DSYNC, 00664);
    if (fd < 0)
        {
            important("Error open event file: %s\n", d->eventFileName);
            return;
        }

    /* write the event file output line */
    int n = write(fd, szLine, 1+strlen(szLine));
    if (n <= 0)
        {
            important("Error write event file: %s\n", d->eventFileName);
        }
    
    /* flush and close the file */
    /*  syncfs(fd); */
    
    close(fd);
}

Boolean ReadEventFromFile(DEVICE d, struct Timestamp *timedate)
{
    Boolean dataexists = TRUE;
    
    /* open the output log file for read */
    FileDesc fd = open(d->eventFileName, O_RDONLY);
    if (fd < 0)
        {
            important("Error open file: %s\n", d->eventFileName);
            dataexists = FALSE;
        }
    else
        {
            /* read the log file output line */
            char szLine[MAX_FILENAME_LENGTH];
            int n = read(fd, szLine, sizeof(szLine)-1);
            if (n <= 0)
                {
                    important("Error read file: %s\n", d->eventFileName);
                    dataexists = FALSE;
                }
            else
                {
                    szLine[n] = '\0';
                }
            close(fd);
            
            if (dataexists)
                {
                    /* get the time and date in the right formats */
                    int n = sscanf(szLine, EVENT_FILE_FORMAT,
                                   &timedate->year, &timedate->mon, &timedate->day,
                                   &timedate->hour, &timedate->min, &timedate->sec);
                    if (n != 6)
                        {
                            important("data in %s of wrong format (%s)\n",
                                      d->eventFileName, szLine);
                            dataexists = FALSE;
                        }
                }
        }
    return(dataexists);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* to send messages, we need to build them up, one string at a time 
   until we have the entire message.  That way, we know how long they
   are, which we need when we send them.  To make this easy, we define
   a buffer structure and a function to add a string to the end of the
   buffer.  When you are done, we will give you a pointer to the
   buffer contents. */
/* the buffer grows as needed for the messages, but we keep the old
   buffer around, so that once we allocate the memory, we just keep
   re-using it. */

struct BUFFER
{
    int  n;
    int  length;
    STRING b;
};

struct BUFFER b = { 0, 0, NULL };

struct BUFFER *ClearBuffer(void)
{
    /* clear the buffer for new use */
    if (b.length <= 0)
        {
            b.length = 1024;
            b.b = CAST(STRING, malloc(b.length));
        }
    b.n = 0;
    b.b[b.n] = '\0';
    return(&b);
}

void AppendBuffer(struct BUFFER *b, STRING s)
{
    if ((s == NULL) || (*s == '\0')) return;
    
    int n = strlen(s);
    /* check that we have room for the string */
    if ((b->n + n + 1) > b->length)
        {
            b->length = 2*b->length;
            b->b = CAST(STRING, realloc(b->b, b->length));
        }
    /* copy the string to the end */
    int i;
    for (i = 0; i < n; i++)
        {
            b->b[b->n] = s[i];
            b->n += 1;
        }
    /* terminate the new string */
    b->b[b->n] = '\0';
}

void QAppendBuffer(struct BUFFER *b, STRING s)
{
    AppendBuffer(b,"\"");
    AppendBuffer(b,s);    
    AppendBuffer(b,"\"");    
}


STRING FinishBuffer(struct BUFFER *b)
{
    if (b->b == NULL) return(NULL);
    STRING s = remember_string(b->b);
    return(s);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we want to create, in memory, in a buffer, the complete message
   (so we know how long it is). */

/* There are two XML messages we create: the response and the update messages. 
   Most of those are the same, so we use a couple of routines to do the
   formatting. */

void AppendHeader(struct BUFFER *buffer, STRING MyRefId, STRING icdVersion)
{
    AppendBuffer(buffer, "<refId>");
    AppendBuffer(buffer, MyRefId);
    AppendBuffer(buffer, "</refId>");
    AppendBuffer(buffer, "<icdVersion>");
    AppendBuffer(buffer, icdVersion);
    AppendBuffer(buffer, "</icdVersion>");
}

void AppendId(struct BUFFER *buffer, DEVICE d)
{
    AppendBuffer(buffer, "<id");
    AppendBuffer(buffer, " providerName=");
    QAppendBuffer(buffer, d->providerName);
    AppendBuffer(buffer, " resourceType=");
    QAppendBuffer(buffer, d->resourceType);
    AppendBuffer(buffer, " centerId=");
    QAppendBuffer(buffer, d->centerId);
    AppendBuffer(buffer, ">");
    AppendBuffer(buffer, d->id);    
    AppendBuffer(buffer, "</id>");
}

void AppendOverheight(struct BUFFER *buffer, DEVICE d, struct Timestamp *timedate, Boolean dataexists)
{
    STRING opStatus = Format_Device_Status(d->status);
    
    AppendBuffer(buffer, "<overheight>");
    if (dataexists)
        {
            char readingTime[16];
            char readingDate[16];
            snprintf(readingTime, sizeof(readingTime), "%02lu:%02lu:%02lu",
                     timedate->hour, timedate->min, timedate->sec);
            snprintf(readingDate, sizeof(readingDate), "%04lu-%02lu-%02lu",
                     timedate->year, timedate->mon, timedate->day);

            AppendBuffer(buffer, "<overheightReadingData>");
            AppendBuffer(buffer, "<readingTime>");
            AppendBuffer(buffer, readingTime);
            AppendBuffer(buffer, "</readingTime>");
            AppendBuffer(buffer, "<readingDate>");
            AppendBuffer(buffer, readingDate);
            AppendBuffer(buffer, "</readingDate>");
            AppendBuffer(buffer, "<triggerHeight units=");
            QAppendBuffer(buffer, "in");
            AppendBuffer(buffer, ">");                        
            AppendBuffer(buffer, d->triggerHeight);
            AppendBuffer(buffer, "</triggerHeight>");
            AppendBuffer(buffer, "</overheightReadingData>");
        }
    AppendBuffer(buffer, "<overheightStatus>");
    AppendBuffer(buffer, "<opStatus>");
    AppendBuffer(buffer, opStatus);
    AppendBuffer(buffer, "</opStatus>");
    AppendBuffer(buffer, "</overheightStatus>");
    AppendBuffer(buffer, "</overheight>");
}


/* we have an event and want to send a message saying so.
   The XML message looks like:

<overheightUpdateMsg>
	<refId> aaa </refId>
	<icdVersion> bbb </icdVersion>
	<id>
		<providerName> fff1 </providerName>
		<resourceType> fff2 </resourceType>
		<centerId> fff3 </centerId>
	</id>
	<overheight>
		<overheightStatus>
			<opStatus> ggg </opStatus>
		</overheightStatus>
		<overheightReadingData>
		    <readingTime> iii </readingTime>
			<readingDate> jjj </readingDate>
			<triggerHeight> kkk </triggerHeight>
	    </overheightReadingData>
	</overheight>
</overheightUpdateMsg>
*/

STRING Format_One_Event_Message(DEVICE d, struct Timestamp *timedate, Boolean dataexists)
{
    /* first get STRING forms of the important things */
    char MyRefId[16];

    myRefId += 1;
    snprintf(MyRefId, sizeof(MyRefId), "%d", myRefId);
    
    struct BUFFER *buffer = ClearBuffer();
    AppendBuffer(buffer, "<overheightUpdateMsg>");
    AppendHeader(buffer, MyRefId, icdVersion);
    AppendId(buffer, d);
    AppendOverheight(buffer, d, timedate, dataexists);
    AppendBuffer(buffer, "</overheightUpdateMsg>");
    STRING b = FinishBuffer(buffer);
    return(b);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we expect a request message that looks like:
   
   <retrieveDataReq>
   <refId> aaa </refId>
   <icdVersion> bbb </icdVersion>
   <overheightData> TRUE </overheightData>
   </retrieveDataReq>
   
   We need to keep the refId, and icdVersion,
   and knowledge that overheightData is true.

   I guess the way to do it is to define a
   set of key/value pairs.  When we get an
   actual value, we define the tag as the
   key and store the value as a STRING.

   In general, we build a tree of
   key/value pair lists.  A leaf of the
   tree is a key/value pair.
   Interior nodes of the tree just
   have a key and a pointer to another
   node.

*/
/* for parsing an incoming XML message, we will need a tree.
   The tree is either a key/value pair, or a key
   and a pointer to another node. We don't use both
   value and xml_list at the same time (they could be
   a union to save space, but that really seems like
   over-kill.
*/

struct xml_element
{
    struct xml_element *next /* peer nodes */;
    STRING key;

    /* the "value" is either a string, or another XML element */
    struct xml_element *xml_list /* child node */;
    STRING value;
};


STRING search_xml_value(struct xml_element *list, STRING key)
{
    struct xml_element *p;
    p = list;
    while (p != NULL)
        {
            if (mystrcasecmp(key, p->key))
                return(p->value);
            p = p->next;
        }
    return(NULL);
}

void dump_xml_element(struct xml_element *root, int level)
{
    int i;
    struct xml_element *element;
    
    for (element = root; element != NULL; element = element->next)
        {
            if (element->value == NULL)
                {
                    for (i = 0; i < level; i++) fprintf(stderr, "    ");
                    fprintf(stderr, "%s:\n", element->key);
                    dump_xml_element(element->xml_list, level+1);
                }
            else
                {
                    for (i = 0; i < level; i++) fprintf(stderr, "    ");                    
                    fprintf(stderr, "%s: %s\n", element->key, element->value);
                }
        }
}

void free_xml_element(struct xml_element *root)
{
    struct xml_element *element;

    element = root;
    while (element != NULL)
        {
            free(element->key);
            if (element->value != NULL) free(element->value);
            if (element->xml_list != NULL)
                free_xml_element(element->xml_list);

            struct xml_element *next = element->next;
            free(element);
            element = next;
        }
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

Boolean is_XML_comment(STRING buffer)
{
    /* XML comment looks like <!--...--> */    
    if ((buffer[0] == '<')
        && (buffer[1] == '!')
        && (buffer[2] == '-')
        && (buffer[3] == '-')) return(TRUE);
    return(FALSE);
}

Boolean is_XML_comment_end(STRING buffer)
{
    if ((buffer[0] == '-')
        && (buffer[1] == '-')
        && (buffer[2] == '>')) return(TRUE);
    return(FALSE);
}

STRING find_end_of_comment(STRING buffer)
{
    /* buffer points at a comment; skip it */
    /* XML comment looks like <!--...--> */

    /* skip opening part; we know it matches exactly */
    buffer = buffer+4;
    while (!is_XML_comment_end(buffer))
        {
            if (*buffer == '\0') return(NULL);
            buffer++;
        }
    buffer += 3;
    return(buffer);
}

Boolean is_XML_declaration(STRING buffer)
{
    /* XML declaration looks like  <?xml...?> */
    if ((buffer[0] == '<')
        && (buffer[1] == '?')
        && (buffer[2] == 'x')
        && (buffer[3] == 'm')
        && (buffer[4] == 'l')) return(TRUE);
    return(FALSE);
}

Boolean is_XML_declaration_end(STRING buffer)
{
    if ((buffer[0] == '?')
        && (buffer[1] == '>')) return(TRUE);
    return(FALSE);
}

STRING find_end_of_declaration(STRING buffer)
{
    /* buffer points at a declaration; skip it */
    /* XML declaration looks like  <?xml...?> */
    buffer = buffer+5;
    while (!is_XML_declaration_end(buffer))
        {
            if (*buffer == '\0') return(NULL);
            buffer++;
        }
    buffer += 2;
    return(buffer);
}



/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

STRING find_end_of_tag(STRING buffer)
{
    /* find end of tag */
    while (istagchar(*buffer)) buffer++;
    /* if we found the end of the input */
    if (*buffer == '\0') return(NULL);
    
    /* terminate the tag as a string */
    if (*buffer == '>')
        {
            *buffer = '\0';
        }
    else
        {
            *buffer = '\0';
            buffer++;            
            /* it appears we have attributes.  In this application of
               XML, so far, we have no need for attributes -- skip them */
            while ((*buffer != '>') && (*buffer != '\0')) buffer++;
            if (*buffer == '\0') return(NULL);
        }
    
    /* advance past the '>' to the next significant character */
    buffer++;                        
    while ((*buffer != '\0') && isspace(*buffer)) buffer++;
    return(buffer);
}


STRING get_value(STRING buffer, STRING *value)
{
    /* We have found something other than a tag,
       capture that value (until the next '<') 
       and advance the buffer pointer */
    /* skip any leading blanks */
    while ((*buffer != '\0') && isspace(*buffer))
        buffer++;

    STRING begin = buffer;

    /* find next tag  (or end of string) */
    while ((*buffer != '<') && (*buffer != '\0'))
        buffer++;

    if (*buffer == '\0')
        {
            *value = remember_string(begin);
        }
    else
        {
            /* if we found the start of a tag */
            /* temporarily change the < to \0, so
               that we terminate the value part,
               save the string, then put the < back */
            *buffer = '\0';
            *value = remember_string(begin);
            *buffer = '<';            

            /* trim leading and trailng spaces */
            STRING v = *value;
            int n = strlen(v);
            while ((n > 0) && isspace(v[n-1]))
                {
                    n -= 1;
                    v[n] = '\0';
                }
        }

    return(buffer);
}


STRING parse_xml_element(STRING buffer, struct xml_element *tag_tree)
{
    /* skip any leading blanks */
    while ((*buffer != '\0') && isspace(*buffer)) buffer++;
    if (*buffer == '\0') return(NULL);

    if (*buffer != '<')	       /* string value */
        {
            buffer = get_value(buffer, &(tag_tree->value));
            return(buffer);
        }
    
    /* XML comments are skipped; start over as if not here */
    if (is_XML_comment(buffer))
        {
            buffer = find_end_of_comment(buffer);
            return(parse_xml_element(buffer, tag_tree));
        }

    /* XML declarations are treated as comments for this application. */
    if (is_XML_declaration(buffer))
        {
            buffer = find_end_of_declaration(buffer);
            return(parse_xml_element(buffer, tag_tree));
        }

    /* another tag.  parse <tag> ... </tag> */
    /* Advance past the < */
    buffer++;

    /* Now we have something that we need to pay attention to */
    /* not a comment or a declaration, but a real tag. */
    
    STRING tag = buffer;
    buffer = find_end_of_tag(buffer);
    if (*buffer == '\0') return(NULL);
    tag_tree->key = remember_string(tag);

    /* new tag */
    if (*buffer != '<')
        {
            /* value of this item is a string */
            buffer = get_value(buffer, &(tag_tree->value));
        }
    else
        {
            /* value of this item is a list of new elements */
            /* accumulate a sequence of tags, until we finally get /tag */
            while ((*buffer == '<') && (buffer[1] != '/'))
                {
                    struct xml_element *tree = TYPED_MALLOC(struct xml_element);
                    tree->xml_list = NULL;            
                    tree->value = NULL;

                    tree->next = tag_tree->xml_list;
                    tag_tree->xml_list = tree;
                    /* recursively parse the rest of the message */
                    buffer = parse_xml_element(buffer, tree);
                    if ((buffer == NULL) || (*buffer == '\0')) return(NULL);                    
                }
       }

    /* now we have (should have) an end tag.  See if it matches. */
    if (*buffer == '<') buffer++; /* skip < */
    if (*buffer == '/') buffer++; /* skip / */    
    if (*buffer == '\0') return(NULL);

    STRING end_tag = buffer;
    buffer = find_end_of_tag(buffer);
    if (!mystrcasecmp(end_tag, tag))
        fprintf(stderr, "tags do not match: <%s> ... </%s>\n", tag, end_tag);

    return(buffer);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


STRING Format_XML_Response(STRING refID, STRING CVM_icdVersion)
{
    /* we are asked to send back the data for the last
       events. Read them from the files, if there are any. */

    /* if we get an icdVersion, which differs from what we
       have, keep their version. */
    if (!mystrcasecmp(icdVersion, CVM_icdVersion))
        UPDATE_STRING(icdVersion, CVM_icdVersion);

    struct BUFFER *buffer = ClearBuffer();
    AppendBuffer(buffer, "<retrieveDataResp xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");    
    AppendHeader(buffer, refID, icdVersion);
    AppendBuffer(buffer, "<data xsi:type=\"retrieveData\">");
    
    int i;
    for (i = 0; i < MAX_DETECTORS; i++)
        {
            DEVICE d = DDD[i];
            if (d == NULL) continue;
            
            struct Timestamp timedate;
            Boolean dataexists = ReadEventFromFile(d, &timedate);

            AppendBuffer(buffer, "<overheightData>");    
            AppendId(buffer, d);
            AppendOverheight(buffer, d, &timedate, dataexists);
            AppendBuffer(buffer, "</overheightData>");
        }
    AppendBuffer(buffer, "</data>");        
    AppendBuffer(buffer, "</retrieveDataResp>");
    STRING b = FinishBuffer(buffer);
    return(b);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* first parse it into a tree, then if
   it is the right type of message, generate a
   reply message. */

STRING Parse_XML_Message(STRING buffer)
{
    STRING message = NULL;
    
    struct xml_element *root = TYPED_MALLOC(struct xml_element);
    root->next = NULL;
    root->key = NULL;
    root->xml_list = NULL;
    root->value = NULL;
    
    /* parse */
    buffer = parse_xml_element(buffer, root);

    if (debug) dump_xml_element(root, 0);

    /* act */
    if ((root->key != NULL) && mystrcasecmp(root->key, "retrieveDataReq"))
        {
            STRING v = search_xml_value(root->xml_list, "overheightData");
            if ((v != NULL) && mystrcasecmp(v, "true"))
                {
                    STRING refID = search_xml_value(root->xml_list, "refId");
                    STRING icdVersion = search_xml_value(root->xml_list, "icdVersion");

                    /* create an XML overheight data message and send it */
                    message = Format_XML_Response(refID, icdVersion);
                }
        }

    free_xml_element(root);
    
    return(message);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

#include <sys/types.h>    /* send, recv, ... */
#include <sys/socket.h>   /* socket, bind, accept, ... */
#include <arpa/inet.h>    /* htons, ... */

#define INVALID_SOCKET  (-1)

/* all our configuration variables are strings.  If we want
   integers, we need to convert them. */

void Initialize_Network_Config_Values(void)
{
    if (StringMyRefId != NULL)
        {
            myRefId = atoi(StringMyRefId);
        }

    
    if (PortName == NULL)
        PortName = "3080";
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


int Send_Request_Message(FileDesc SocketFD, STRING buffer)
{
    int rc;

    int n = strlen(buffer);

    important("outgoing message:\n(%d)(%d)%s\n", n, 0, buffer);

    /* Send our request */
    /* Sending, like receiving, requires first the 
       big-endian number of bytes, then a second 
       reserved word, then the message. */

    /* first word is length */
    int i;
    int shift = 32;
    UINT8 buf[4];
    for (i = 0; i < 4; i++)
        {
            shift = shift - 8;
            buf[i] = (n >> shift) & 0xFF;
        }
    rc = send(SocketFD, buf, 4, 0);    
    if ((rc < 0) || (rc != 4))
        {
            important("send of first 4 bytes (0X%02X,0X%02X,0X%02X,0X%02X) fails\n",
                    buf[0],buf[1],buf[2],buf[3]);
            perror("send");
            return(-7);
        }

    /* second word of zeros */
    for (i = 0; i < 4; i++) buf[i] = 0;
    rc = send(SocketFD, buf, 4, 0);    
    if ((rc < 0) || (rc != 4))
        {
            important("send of second 4 bytes (0X%02X,0X%02X,0X%02X,0X%02X) fails\n",
                    buf[0],buf[1],buf[2],buf[3]);
            perror("send");
            return(-6);
        }

    /* then send the right number of message bytes */
    rc = send(SocketFD, buffer, n, 0);
    if (rc < 0)
        {
            important("send of %d bytes fails\n", n);
            perror("send");
            return(-5);
        }

    if (rc < n)
        {
            important("send() sent only %d out of %d bytes\n", rc, n);
            perror("send");
            return(-10);
        }

    return(0);
}




/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* We set up a socket for others to connect to. */

#define BACKLOG 5


FileDesc Initialize_for_Network_Requests(void)
{
    FileDesc ConnectionSocket;
    struct sockaddr_in  sin;

    /* create the connection socket and set its parameters of use */
    ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ConnectionSocket < 0)
        {
            perror("socket");
            return(INVALID_SOCKET);
        }

    (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_REUSEADDR,   (char *)NULL, 0);

    /* define the name and port to be used with the connection socket */
    bzero((char *)&sin, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    int port = CAST(unsigned short, atoi(PortName)); /* get the port number */
    sin.sin_port = htons(port);

    /* bind the name and port number to the connection socket */
    if (bind(ConnectionSocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
            perror("bind");
            return(INVALID_SOCKET);
        }

    important("Socket is FD %d for port %d\n", ConnectionSocket, port);

    /* now activate the named connection socket to get messages */
    if (listen(ConnectionSocket, BACKLOG) < 0)
        {
            perror("listen");
            return(INVALID_SOCKET);
        };

    important("Listening on FD %d\n", ConnectionSocket);
    return(ConnectionSocket);
}

FileDesc Accept_Client(FileDesc ConnectionSocket)
{
    /* wait for a connection from a Client */
    
    FileDesc ClientFD;
    struct sockaddr_in  from;
    socklen_t   len = sizeof (from);
    
    ClientFD = accept(ConnectionSocket, (struct sockaddr *)&from, &len);
    if (ClientFD < 0)
        {
            important("NewConnection: error %d\n", errno);
            return(INVALID_SOCKET);
        }

    important("Connect To Client: FD %d \n", ClientFD);
    return(ClientFD);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* We have two file descriptors to worry about:
   
   ServerConnection -- the socket that we create to allow clients
                       to create a connection.  We create the
                       socket(), bind(), and then listen().  This is
                       done once, and we keep that connection open
                       for CVM to attach to when it wants.

   ClientConnection -- the actual connection between us and CVM.
                       CVM will connect to our ServerConnection,
                       and we will do an accept() to create the
                       connection.  This connection may go away,
                       and be recreated as necessary.
   
 */

FileDesc ServerConnection = INVALID_SOCKET;
FileDesc ClientConnection = INVALID_SOCKET;


void close_Client_Connection(void)
{
    important("close client: FD %d\n", ClientConnection);
    close(ClientConnection);
    ClientConnection = INVALID_SOCKET;
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

STRING Read_XML_Message(void)
{
    int i;
    int rc;    
    /* The request/message format is defined by the CVM-VCS-Protocol
       as 
       4 bytes -- bigendian number of bytes in messages (n)
       4 bytes -- reserved for future use (0)
       n bytes -- XML message
    */
    
    /* read the length */
    int n = 0;
    for (i = 0; i < 4; i++)
        {
            int c;
            rc = recv(ClientConnection, &c, 1, MSG_WAITALL);
            if (rc <= 0)
                {
                    important("message truncated first 4 bytes\n");
                    close_Client_Connection();
                    return(NULL);
                }
            n = (n << 8) | (c & 0xFF);
        }
    if (debug) fprintf(stderr, "message of %d bytes\n", n);
    if (n <= 0)
        {
            important("body of message missing\n");            
            close_Client_Connection();
            return(NULL);
        }

    /* read the 2nd byte */
    int m = 0;
    for (i = 0; i < 4; i++)
        {
            int c;
            rc = recv(ClientConnection, &c, 1, MSG_WAITALL);
            if (rc <= 0)
                {
                    important("message truncated second 4 bytes\n");
                    close_Client_Connection();                    
                    return(NULL);
                }
            m = (m << 8) | (c & 0xFF);
        }
    if ((m != 0) && debug) fprintf(stderr, "message 2nd byte is 0x%08X\n", m);

    if (n > MAX_MESSAGE_LENGTH) return(NULL);

    /* allocate a memory buffer for the message */
    STRING buffer = CAST(STRING, malloc(n+1));
    rc = recv(ClientConnection, buffer, n, MSG_WAITALL);
    if (rc != n)
        {
            important("recv() failed for XML; should have been %d bytes, but only %d\n", n, rc);
            perror("recv");
            close_Client_Connection();                            
            free(buffer);
            return(NULL);
        }

    /* be sure the buffer is zero-terminated */
    buffer[n] = '\0';

    important("incoming message:\n(%d)(%d)%s\n", n, m, buffer);
    
    return(buffer);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void WriteXMLMessageToServer(DEVICE d, struct Timestamp *timedate, Boolean dataexists)
{
    /* create an XML overheight data message and send it */
    STRING message = Format_One_Event_Message(d, timedate, dataexists);
    if (message != NULL)
        {
            /* to send a message, we need an open connection to CVM */
            if (ClientConnection != INVALID_SOCKET)
                {
                    int rc = Send_Request_Message(ClientConnection, message);
                    if (rc < 0)
                        {
                            important("XML event message fails\n");
                            close_Client_Connection();                            
                        }
                }
            free(message);
        }
}


void Read_and_Reply_to_CVM(void)
{
    STRING buffer = Read_XML_Message();
    
    /* see if the message requires a response */
    if (buffer != NULL)
        {
            STRING message = Parse_XML_Message(buffer);
            free(buffer);
            
            if (message == NULL)
                {
                    important("XML message does not require response\n");                    
                }
            else
                {
                    int rc = Send_Request_Message(ClientConnection,  message);
                    if (rc < 0)
                        {
                            important("XML response message fails\n");
                            close_Client_Connection();
                        }
                    free(message);
                }
        }
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

void Setup_for_Network_Requests(void)
{
    /* We want to create a socket to the server and wait for
       a request from them.  Process that request and
       send back a reply.
    */

    Initialize_Network_Config_Values();

    ServerConnection = Initialize_for_Network_Requests();
    if (ServerConnection == INVALID_SOCKET)
        {
            important("cannot establish server socket\n");
            exit(-1);
        }
}


void Finish_for_Network_Requests(void)
{
    if (ClientConnection != INVALID_SOCKET)    
        close(ClientConnection);
    if (ServerConnection != INVALID_SOCKET)    
        close(ServerConnection);
}
            
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The following code is modelled on code that was provided by Moxa to
   show how to get a signal from the hardware. The code has almost no
   useful comments, and the documentation is semantic content free, so
   you sort of have to guess what is being done.

   At first it looked like the "dio_event" concept would work well
   here, but a dio_event maps to a particular input wire, and we
   need at least two (one for the event, one for the fault condition)
   for each detector device.  So we couldn't use the dio_event.
   It appears the only other choice we have is to explicitly poll
   and read all the DI bits repeatedly.  

   Our configuration file should build a table that tells us
   for each possible channel (0 to 7) if it has anything 
   attached, and if so, what detector device.  In addition,
   we need to know if it is an event wire or a fault wire.
*/


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */


/* all our configuration variables are strings.  If we want
   integers, we need to convert them. */

void Initialize_DI_Config_Values()
{
}


                  
void Process_Actual_DI_Event(DEVICE d)
{
    /* get the current date and time */
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    struct Timestamp timedate;
    timedate.year = tm->tm_year + 1900;
    timedate.mon = tm->tm_mon + 1;
    timedate.day = tm->tm_mday;    
    timedate.hour = tm->tm_hour;
    timedate.min = tm->tm_min;
    timedate.sec = tm->tm_sec;

    WriteEventToFile(d, &timedate);
    WriteXMLMessageToServer(d, &timedate, TRUE);                    
}

void Process_Change_In_Status_Event(DEVICE d)
{
    struct Timestamp timedate;
    Boolean dataexists = ReadEventFromFile(d, &timedate);
    WriteXMLMessageToServer(d, &timedate, dataexists);                    
}


void setStatus(DEVICE d, enum DeviceStatus status)
{
    if (d->status != status)
        {
            d->status = status;
            Process_Change_In_Status_Event(d);
        }
    important("Device %s Status set to %s\n", d->name, Format_Device_Status(status));
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we have an input wire that has changed.  Do the right thing. */
void Process_DI_Event(int channel, int new_state)
{
    /* first determine if this wire is attached to a detector device */
    DEVICE d = Channel_Table[channel];
    if (d == NULL)
        {
            important("We had a bogus signal on channel %d\n", channel);
            return;
        }

    /* check if we have a new event */
    if (d->event_channel == channel)
        {
            /* events are only interesting when they start, not
               when they end.  So check if the new_state is 1. */
            if (new_state != 1) return;

            Process_Actual_DI_Event(d);
        }

    if (d->fault_channel == channel)
        {
            /* a device that is out of service does not
               change state until it is back in service. */
            if (d->status == ST_OUTOFSERVICE) return;

            /* the fault channel can change us from ACTIVE
               to FAILED, or back again. */
            if (new_state == 1)
                setStatus(d, ST_FAILED);
            else
                setStatus(d, ST_ACTIVE);                
        }

    
}

    

/* set things up so that we can poll the input lines. */
void Setup_for_IO_Polling(void)
{
    int rc;
    int i;
    
	/* Set all DI channel mode to DI */
    UINT8 chMode[MAX_CHANNELS];
	for (i = 0; i < MAX_CHANNELS; i++)
        {
            chMode[i] = DI_MODE_DI;
        }

	rc = MX_RTU_Module_DIO_DI_Mode_Set(diSlot, 0, MAX_CHANNELS, chMode);

    if (rc != MODULE_RW_ERR_OK)
        {
            important("MX_RTU_Module_DIO_DI_Mode_Set err:%d\n", rc);
        }
}

void Finish_for_IO_Polling(void)
{
}

void Poll_for_DI_Event()
{
    static UINT32 last_diValue = 0;
    UINT32 diValue;

    int rc;
    struct Timestamp timedate;
    
    /*  We want to read the DI input lines, and see if they have changed. */
    rc = MX_RTU_Module_DI_Value_Get(diSlot, &diValue, &timedate);
    if(rc != MODULE_RW_ERR_OK)
        {
            important("MX_RTU_Module_DIO_DI_Value_Get err:%d\n", rc);
            return;
        }

    /* quick check to see if anything has changed.  If not, we done. */
    if (diValue == last_diValue) return;

    if (debug) fprintf(stderr, "DI value has changed: 0x%08lX -> 0x%08lX\n", last_diValue, diValue);

    /* Something seems to have changed.  Figure out what,
       and do the right thing. */
    /* Although it never says anywhere what the assignment of
       bits to wires is, the sample code all do (1 << i) to
       get the state of channel i (i = 0..7). */
    int i;
    for (i = 0; i < MAX_CHANNELS; i++)
        {
            UINT32 mask = (1 << i);
            if ((diValue & mask) != (last_diValue & mask))
                {
                    /* channel i has changed state */
                    int new_state = (diValue >> i) & 1;
                    Process_DI_Event(i, new_state);
                }
        }

    /* remember the updated state of the inputs */
    last_diValue = diValue;
}




/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* Adding Signal processing */

/* we handle the following signals:

   SIGUSR1 -- simulate an overhead event.
   SIGUSR2 --
   SIGPWR -- put out of service
   SIGCONT -- put back in service.
*/

void sig_Overhead_Event_0(int signo)
{
    /* now act like this was an overhead event */
    DEVICE d = DDD[0];
    if (d != NULL)
        Process_Actual_DI_Event(d);
}


void sig_Overhead_Event_1(int signo)
{
    /* now act like this was an overhead event */
    DEVICE d = DDD[1];
    if (d != NULL)
        Process_Actual_DI_Event(d);
}


void sig_refresh(int signo)
{
    (void)Read_Config_File();
    if (verbose)
        Dump_Program_State();
}

void sig_fail(int signo)
{
    DEVICE d = DDD[0];
    if (d != NULL)
        {
            if (d->status == ST_FAILED)
                setStatus(d,ST_ACTIVE);
            else
                setStatus(d,ST_FAILED);
        }
}


void Setup_Signal_Handlers(void)
{
    signal(SIGUSR1, sig_Overhead_Event_0);
    signal(SIGUSR2, sig_Overhead_Event_1);
    signal(SIGPWR, sig_refresh);
    signal(SIGFPE, sig_fail);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The main loop of the program.  We wait for either
   network activity or a polling time-out, and then do
   the appropriate thing in response.  This goes on forever. */

void main_loop(void)
{
    fd_set rfds, wfds, xfds;
    /* time out at least once a day (tv_sec: seconds, tv_usec: microseconds) */
    static struct timeval TimeOut;
    struct timeval *Timer = &TimeOut;

    while (TRUE)
        {
            /* we will wait for input from the general connection
               socket (ServerConnection), or if a connection to
               CVM has been made, for input from them
               (ClientConnection), or for a polling time-out. */
            FD_ZERO(&rfds);

            FileDesc max_fd = ServerConnection;
            FD_SET(ServerConnection, &rfds);

            if (ClientConnection != INVALID_SOCKET)
                {
                    if (ClientConnection > max_fd) max_fd = ClientConnection;
                    FD_SET(ClientConnection, &rfds);
                }

            FD_ZERO(&wfds);
            xfds = rfds;

            /* set the polling time-out, in seconds and microseconds */
            Timer->tv_sec = 0;
            Timer->tv_usec = pollingDelay;            

            /* wait for input */
            int rc = select(max_fd + 1, &rfds, &wfds, &xfds, Timer);
            // if (debug) fprintf(stderr,"select(...) = 0x%X\n", rc);
            
            /* check for error */
            if (rc < 0)
                {
                    /* check for a signal; just continue if so */
                    if (errno == EINTR) continue;
                    
                    important("error on socket select\n");
                    perror("select");
                    return;
                }

            /* check for time-out */
            if (rc == 0)
                {
                    Poll_for_DI_Event();
                    continue;
                }

            /* see if we have CVM wanting to talk to us */
            if (FD_ISSET(ServerConnection, &rfds))
                {
                    ClientConnection = Accept_Client(ServerConnection);
                }

            /* only other possibility is we have a message ! */            
            if ((ClientConnection != INVALID_SOCKET) && FD_ISSET(ClientConnection, &rfds))
                {
                    Read_and_Reply_to_CVM();
                }
        }
    
}


/*******************************************************************************
 *
 * Main code to start everything up.  
 *    Read the command line arguments.
 *    Read the config file.
 *    Fork (a) the event reader and (b) the XML request processor.
 *
 ******************************************************************************/


int main(int argc, char **const argv)
{
    Scan_Command_Line_Arguments(argc, argv);

    /* we do not know where we will be started, so we
       will cd to the right directory before we start */
    int rc = chdir(Home_Directory);
    if (rc != 0)
        {
            fprintf(stderr, "Can not chdir to home directory: %s\n", Home_Directory);
            perror(Home_Directory);
            exit(errno); 
        }
        
    if (!Read_Config_File())
        return(-1);

    Setup_for_Logging();
    Setup_for_Network_Requests();
    Setup_for_IO_Polling();
    Setup_Signal_Handlers();    
    

    main_loop();

    Finish_for_IO_Polling();
    Finish_for_Network_Requests();
    
    fclose(log_file);
}

