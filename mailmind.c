#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "max/max.h"
#include "squish/h/msgapi.h"     // Squish MsgAPI header file (required for all MsgAPI use)


// TAG_MAX defines the maximum length of a tag (including terminating NULL) identifying the message area
#define TAG_MAX     64


struct _marea_lastprocessed
{
    char tag[TAG_MAX];      // msg areas are referenced by their tag (set in Maximus msgarea.ctl)

    UMSGID last_umsgid;     //  umsgid of the last message processed in this area
};


struct _marea_list
{
// msg areas are referenced by their tag (set in Maximus msgarea.ctl)
    char tag[TAG_MAX];

    struct _marea_list *next;
};


// maximum length of a user name (Maximus internal limit is 36)
#define USERNAME_MAX    36

#define EMAIL_MAX       80

struct _user
{
    char username[USERNAME_MAX];        // could there be multiple username/alias matches to a message?
    char alias[USERNAME_MAX];           // what is best way to handle multiple names? allow more than one?  one + alias?
// update username/alias from USER.BBS file automatically?  there are some reserved fields too, but maybe sketchy to use them?
// could use something like char dataphone[19] in user.bbs to track individual users even if name changes?
    char email[EMAIL_MAX];              // email could be attached to more than one user? so if 'unsubscribe' received, make sure to mark them all
//       byte rsvd6[54];       /* Reserved for future use                   406*/
//             ^^^^ this could be used to store email inside USER.BBS? still need the other fields   
    int unsubscribed;

// default to including all msg areas
// if any areas are listed in include_mareas, then only include those areas
// if any areas are listed in exclude_mareas, then skip those areas
    struct _marea_list *include_mareas;
    struct _marea_list *exclude_mareas;
};


// https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
// Sep 23 '08 at 18:12, Adam Rosenfield
// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}


int search_for_new_mail( MAREA *marea, 
                        unsigned char **marea_heap_ptr, int num_marea, 
                         struct _usr *usr, int NumUserBbs, struct _user *MailUsers, int NumMailUsers, 
                         struct _marea_lastprocessed *marea_lastprocessed, int num_marea_lastprocessed, char *MareaFilename )
{
    int retval;
    struct _minf minf;
    HAREA CurArea;
    HMSG CurMsg;
    long int MsgLen;
#define MsgBufSize      ((unsigned long)32767)
//    unsigned char MsgBuf[MsgBufSize];
    unsigned char *MsgBuf = NULL;
#define MsgCtrlBufSize  ((unsigned long)32767)
//    unsigned char MsgCtrlBuf[MsgCtrlBufSize];
    unsigned char *MsgCtrlBuf = NULL;
    XMSG xmsg;
    UMSGID umsgid;
    char MsgBaseFilename[FILENAME_MAX] = "";
    int AreaIdx;
    int i;
    int n;
    UMSGID last_umsgid;             // last UMSGID processed in this area
    long msgn;
    int MsgCount;
    int found_user_in_db;
    int have_printed_newline;
    FILE *OutFile;
    char OutFilename[FILENAME_MAX] = "d:\\bbs\\mailmind\\out\\";
    char OutFilename_emo[FILENAME_MAX] = "";
    char OutFilename_eao[FILENAME_MAX] = "";
    int MailUserIdx;
    struct _user *NotifyUsers = NULL;
    int NumNotifyUsers = 0;
    int file_idx;
    FILE *MailuserCfgFile;
    int marea_lastprocessed_updated = 0;        // does marea_lastprocessed file need to be written back to disk
    FILE *MareaFile;
    char *MsgTo;


    if( !marea || !marea_heap_ptr || !usr || !MailUsers || ((!marea_lastprocessed) && num_marea_lastprocessed) )
    {
        printf( "error - NULL pointer passed to search_for_new_mail() - aborting.\n" );
        return -1;
    }


    MsgBuf = (unsigned char *)calloc( MsgBufSize, 1 );
    if( !MsgBuf )
    {
        printf( "error - can't allocate %ld bytes for MsgBuf - aborting.\n", MsgBufSize );
        return -2;
    }

    MsgCtrlBuf = (unsigned char *)calloc( MsgCtrlBufSize, 1 );
    if( !MsgCtrlBuf )
    {
        printf( "error - can't allocate %ld bytes for MsgCtrlBuf - aborting.\n", MsgCtrlBufSize );
        if( MsgBuf )
            free( MsgBuf );
        MsgBuf = NULL;
        return -3;
    }


// setup minf struct before initializing Squish MsgApi
    memset( &minf, 0, sizeof(struct _minf) );
    minf.req_version = MSGAPI_VERSION;
    minf.def_zone = 10;

// open MsgApi
    retval = MsgOpenApi(&minf);
    if( retval )
    {
        printf( "error opening Squish MsgApi - MsgOpenApi() failed: %d - aborting.\n", retval );
        if( MsgBuf )
            free( MsgBuf );
        MsgBuf = NULL;
        if( MsgCtrlBuf )
            free( MsgCtrlBuf );
        MsgCtrlBuf = NULL;
        return -4;
    }

    
// loop through all areas in marea[] and check for new messages in each    
    for( AreaIdx=0; AreaIdx<num_marea; AreaIdx++ )
    {
// open a Squish message base
        printf( "Scan area %s (%s)... ", marea_heap_ptr[AreaIdx]+marea[AreaIdx].echo_tag, marea_heap_ptr[AreaIdx]+marea[AreaIdx].name );
        have_printed_newline = 0;

//        printf( " '%s'", marea_heap_ptr[AreaIdx]+marea[AreaIdx].path );
        strncpy( MsgBaseFilename, marea_heap_ptr[AreaIdx]+marea[AreaIdx].path, sizeof(MsgBaseFilename) );

        CurArea = MsgOpenArea( MsgBaseFilename, MSGAREA_NORMAL, marea[AreaIdx].type );
        if( !CurArea )
        {
            printf( "MsgOpenArea() failed - skipping.\n" );
            continue;
        }

// find last_umsgid if we have previously processed this area, so we know where to start looking at messages
        last_umsgid = 0;
        umsgid = 0;
        for( i=0; i<num_marea_lastprocessed; i++ )
        {
            if( !stricmp( marea_lastprocessed[i].tag, marea_heap_ptr[AreaIdx]+marea[AreaIdx].echo_tag ) )
            {
                last_umsgid = marea_lastprocessed[i].last_umsgid;
                umsgid = last_umsgid;
                break;
            }
        }
        
        msgn = MsgUidToMsgn(CurArea, last_umsgid, UID_PREV);
        if( msgn )
        {
            CurMsg = MsgOpenMsg( CurArea, MOPEN_READ, msgn );
            // no processing to do with this message, it was the previously processed message - just opening it so we can use MSGNUM_NEXT
            if( CurMsg )
                MsgCloseMsg( CurMsg );
        }
        
        MsgCount = 0;           // reset count of # of messages processed in this area
// open next message in this area
        for( CurMsg = MsgOpenMsg( CurArea, MOPEN_READ, MSGNUM_NEXT ); CurMsg; CurMsg = MsgOpenMsg( CurArea, MOPEN_READ, MSGNUM_NEXT ) )
        {
            MsgLen = MsgReadMsg( CurMsg, &xmsg, 0, MsgBufSize, MsgBuf, MsgCtrlBufSize, MsgCtrlBuf );
    
            if( MsgLen >=0 )
            {
                MsgCount++;     // increment count of # of messages processed in this area
          
                MsgTo = trimwhitespace(xmsg.to);
                if( !strlen(MsgTo) )
                    continue;       // if no xmsg.to string exists, skip this message (ie: blank To: field)

                for( MailUserIdx=0; MailUserIdx<NumMailUsers; MailUserIdx++ )
                {
                    found_user_in_db = -1;
                    for( n=0; n<NumUserBbs; n++ )
                    {                            
                        if( !stricmp( MailUsers[MailUserIdx].username, usr[n].name ) )
                        {
                            found_user_in_db = n;
                        }
                    }
//TODO - check that MailUsers[MailUserIdx].username has access to this message area
//       need to parse ACS string to determine proper access level - this is not trivial
                    if( !stricmp( MsgTo, MailUsers[MailUserIdx].username ) || !stricmp( MsgTo, MailUsers[MailUserIdx].alias )
                        || (found_user_in_db && !stricmp( MsgTo, usr[n].alias )) )
                    {   // a message addressed to MailUsers[MailUserIdx] has been found                        
                        if( !have_printed_newline )
                        {
                            printf( "\n" );
                            have_printed_newline = 1;
                        }
                        printf( "T: %s", MsgTo );
                        printf( ", F: %s", xmsg.from );
                        printf( ", %s", xmsg.subj );
                        printf( ", %d-%02d-%02d %02d:%02d:%02d", 1980+xmsg.date_written.date.yr, xmsg.date_written.date.mo, xmsg.date_written.date.da, 
                                                        xmsg.date_written.time.hh, xmsg.date_written.time.mm, xmsg.date_written.time.ss);
                        printf( "\n" );

// add this MailUser to the notify list
                        for( i=0; i<NumNotifyUsers; i++ )
                        {
                            if( &NotifyUsers[i] == &MailUsers[MailUserIdx] )
                                break;
                        }
                        if( i>= NumNotifyUsers )                            
                        {   // MailUsers[MailUserIdx] isn't in NotifyUsers[] - add it
                            NumNotifyUsers++;
                            NotifyUsers = realloc( NotifyUsers, NumNotifyUsers*sizeof(struct _user) );
                            if( NotifyUsers )
                                NotifyUsers[NumNotifyUsers-1] = MailUsers[MailUserIdx];
                            else
                            {
                                printf( "error - unable to allocate %d bytes for NotifyUsers[] - lost notify list.\n", NumNotifyUsers*sizeof(struct _user) );
                                NumNotifyUsers = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                printf( "MsgReadMsg(): Nothing could be read from the message.\n" );
            }
                            
            
// close the current open message            
            retval = MsgCloseMsg( CurMsg );
            if( retval )
            {
                printf( "error - MsgCloseMsg() failed.\n" );
            }
        }

        if( MsgGetCurMsg( CurArea ) )
            umsgid = MsgMsgnToUid( CurArea, MsgGetCurMsg( CurArea ) );
        else
            umsgid = last_umsgid;
        
// close the Squish message base
        retval = MsgCloseArea(CurArea);
        if( retval )
        {
            printf( "error - MsgCloseArea() failed: %d\n", retval );
        }

        
        printf( "Processed %d message%s.\n", MsgCount, MsgCount==1 ? "" : "s" );


        if( umsgid != last_umsgid )
        {
            // update marea_lastprocessed[].last_umsgid for this area
            for( i=0; i<num_marea_lastprocessed; i++ )
            {
                if( !stricmp( marea_lastprocessed[i].tag, marea_heap_ptr[AreaIdx]+marea[AreaIdx].echo_tag ) )
                {
                    marea_lastprocessed[i].last_umsgid = umsgid;
                    break;
                }
            }
            if( i >= num_marea_lastprocessed )
            {
                num_marea_lastprocessed++;
                marea_lastprocessed = realloc(marea_lastprocessed, num_marea_lastprocessed*sizeof(struct _marea_lastprocessed) );
                if( !marea_lastprocessed )
                {
                    printf( "error - unable to allocate %d bytes for marea_lastprocessed.\n", num_marea_lastprocessed*sizeof(struct _marea_lastprocessed) );
                    continue;
                }
                strncpy( marea_lastprocessed[num_marea_lastprocessed-1].tag, marea_heap_ptr[AreaIdx]+marea[AreaIdx].echo_tag, TAG_MAX );
                marea_lastprocessed[num_marea_lastprocessed-1].tag[TAG_MAX-1] = '\0';
            }
            marea_lastprocessed_updated=1;
        }
    }


    for( i=0; i<NumNotifyUsers; i++ )
    {
        printf( "Notify: %s\n", NotifyUsers[i].username );
        
        if( NotifyUsers[i].unsubscribed )
        {
            printf( "User is unsubscribed - not sending notification email.\n" );
            continue;
        }
        
// create email outgoing file
        file_idx=0;
        OutFile = NULL;
        do
        {   // check if filename based on hash of NotifyUsers[i].email already exists
            if( OutFile )
                fclose( OutFile );
            file_idx++;
            sprintf( OutFilename_eao, "%s%08lX.EAO", OutFilename, SquishHash( NotifyUsers[i].email ) + file_idx );
            OutFile = fopen( OutFilename_emo, "rt" );
        }while( OutFile );
        // OutFilename is a filename that doesn't already exist    
        OutFile = fopen( OutFilename_eao, "wt" );
        if( !OutFile )
        {
            printf( "error - unable to open '%s' for writing.\n", OutFilename_eao );
            continue;
        }        
        fprintf( OutFile, "To: %s\n", NotifyUsers[i].email );
        fprintf( OutFile, "From: Another Millennium BBS <anothermillenniumbbs@gmail.com>\n" );
        fprintf( OutFile, "Subject: New mail notification\n" );
        fprintf( OutFile, "\n" );
        fprintf( OutFile, "You have new mail waiting at Another Millennium BBS.\n" );
        fclose( OutFile );

        do
        {   // check if filename based on hash of NotifyUsers[i].email already exists
            if( OutFile )
                fclose( OutFile );
            file_idx++;
            sprintf( OutFilename_emo, "%s%08lX.EMO", OutFilename, SquishHash( NotifyUsers[i].email ) + file_idx );
            OutFile = fopen( OutFilename_emo, "rt" );
        }while( OutFile );

        if( rename( OutFilename_eao, OutFilename_emo ) )
        {
            printf( "error - unable to rename '%s' to '%s'.\n", OutFilename_eao, OutFilename_emo );
        }
    }


    if( marea_lastprocessed_updated )
    {
// write updated marea_lastprocessed database to disk
// open marea.dat file    
        MareaFile = fopen( MareaFilename, "wb" );
        if( MareaFile )
        {
            fwrite( marea_lastprocessed, sizeof(struct _marea_lastprocessed) * num_marea_lastprocessed, 1, MareaFile );
            fclose( MareaFile );
        }
        else
        {
            printf( "Unable to open MAREA.DAT file '%s' for writing - can't update marea_lastprocessed[].umsgid\n", MareaFilename );
        }
    }
    

// close MsgApi    
    retval = MsgCloseApi();
    if( retval )
    {
        printf( "error - MsgCloseApi(): %d\n", retval );
    }

    
// clean up allocated memory no longer used after this function
    if( NotifyUsers )
        free( NotifyUsers );

    if( MsgBuf )
        free( MsgBuf );
    MsgBuf = NULL;
    if( MsgCtrlBuf )
        free( MsgCtrlBuf );
    MsgCtrlBuf = NULL;



    return 0;
}


//--------------------------------------------------------------------
//get next token  - note: t must be already allocated
char *gettoken( char *expstring, char *t, int max_token_len )
{
	unsigned char tidx=0;
	unsigned char inquote=0;

	
	t[0] = '\0';						// start token string (t) as empty string


	while( (*expstring) && tidx<max_token_len )		// loop until end of string is reached
	{
		if( ((*expstring)>='A'&&(*expstring)<='Z') || ((*expstring)>='a'&&(*expstring)<='z') || 
				((*expstring)=='.') || ((*expstring)=='_') || ((*expstring)>='0'&&(*expstring)<='9') )
		{
			t[ tidx++ ] = (*expstring);
			t[ tidx ] = '\0';
		}
		else if( (*expstring)=='\"' )					// quote
		{
			if( inquote )
			{											// got closing quote
				t[ tidx++ ] = (*expstring++);
				t[ tidx ] ='\0';
				return expstring;
			}
			inquote=1;									// got opening quote
			t[ tidx++ ] = (*expstring);
			t[ tidx ] ='\0';
		}
		else if( inquote )
		{
			t[ tidx++ ] = (*expstring);
			t[ tidx ] = '\0';
		}
		else if( ((*expstring)==' ') || ((*expstring)=='\t') )
		{												// whitespace, not inside quotes
			if( tidx )
				return expstring;
		}
		else if( (*expstring)=='\'' )
		{
			return expstring+strlen(expstring);
		}
		else											// symbol
		{
			if( tidx )
				return expstring;
			t[ tidx ] = (*expstring++);
			if( t[tidx] == '<' && expstring[0] == '=' )
			{											// got <=
				expstring++;
				t[ ++tidx ] = '=';
			}
			else if( t[tidx] == '>' && expstring[0] == '=' )
			{											// got >=
				expstring++;
				t[ ++tidx ] = '=';
			}
			else if( t[tidx] == '<' && expstring[0] == '>' )
			{											// got <>
				expstring++;
				t[ ++tidx ] = '>';
			}
			t[ ++tidx ] = '\0';
			return expstring;
		}
		expstring++;
	}

	return expstring;
}

// NOTE - *prm_heap will need to be free()'d by the caller (it is allocated within read_prm())
int read_prm( struct m_pointers *prm, unsigned char **prm_heap, char *PrmFilename )
{
    FILE *PrmFile;
//    struct m_pointers prm;
    unsigned char *heap;
    long filesize;
    long heapsize;
    

    if( !prm || !prm_heap || !PrmFilename )
    {
        printf( "error - NULL pointer passed to read_prm() - aborting.\n" );
        return -1;
    }    
    
// open max.prm    
    PrmFile = fopen( PrmFilename, "rb" );
    if( !PrmFile )
    {
        printf( "Unable to open Maximus .prm file '%s' - aborting.\n", PrmFilename );
        return 1;
    }


    if( !fread( prm, sizeof(struct m_pointers), 1, PrmFile ) )
    {
        printf( "Error reading Maximus .prm file '%s' - aborting.\n", PrmFilename );
        fclose(PrmFile);
        return 2;
    }

//    printf( "prm.id: %02X '%c'\n", prm->id, prm->id>=0x20 && prm->id<=0x7E ? prm->id : ' ' );
//    printf( "prm.version: %0d\n", prm->version );
//    printf( "prm.heap_offset: %04X\n", prm->heap_offset );

    fseek( PrmFile, 0, SEEK_END );
    filesize = ftell( PrmFile );
    heapsize = filesize - prm->heap_offset;
    fseek( PrmFile, prm->heap_offset, SEEK_SET );

//    printf( "calloc( 1, %ld )\n", heapsize );
    heap = calloc( 1, heapsize );
    (*prm_heap) = heap;
    if( !heap )
    {
        printf( "calloc() error - can't allocate %ld bytes.\n", heapsize );
        fclose( PrmFile );
        return 3;
    }
    
    if( !fread( heap, heapsize, 1, PrmFile ) )
    {
        printf( "Error reading Maximus .prm file '%s' heap (%ld bytes) - aborting.\n", PrmFilename, filesize );        
        fclose( PrmFile );
        if( heap )
            free(heap) ;
        return 4;
    }
    
// close max.prm    
    fclose( PrmFile );
//    printf( "Closed Maximus .prm file '%s'.\n", PrmFilename );

//    printf( "System name: '%s'\n", heap+prm->system_name );
//    printf( "Sysop: '%s'\n", heap+prm->sysop );
//    printf( "marea_name: '%s'\n", heap+prm->marea_name );

//    if( heap )
//        free(heap) ;

    
    return 0;
}


// NOTE - *marea_ptrmust be free()'d by caller after use, it will be allocated by read_marea() but is not free()'d here
// NOTE - **marea_heap_ptr must be free()'d by caller after use, it will be allocated by read_marea() but is not free()'d here
//        each of num_marea entries in *marea_heap_ptr must also be free()'d by caller after use
int read_marea( MAREA **marea_ptr, unsigned char ***marea_heap_ptr, char *MareaFilename )
{
    FILE *MareaFile;
    OVERRIDE ovr;           // for our purposes it's ok to just ignore the overrides (read them to keep in sync with file, but don't save the data)
    unsigned char *heap;
    long filesize;
    long fileid;
    int i;
    int num_marea=0;
    MAREA *marea=NULL;


    if( !marea_ptr || !marea_heap_ptr || !MareaFilename )
    {
        printf( "error - NULL pointer passed to read_marea() - aborting.\n" );
        return -1;
    }    

// open marea.dat file    
    MareaFile = fopen( MareaFilename, "rb" );
    if( !MareaFile )
    {
        printf( "Unable to open MAREA.DAT file '%s' - aborting.\n", MareaFilename );
        return -1;
    }

    fseek( MareaFile, 0, SEEK_END );
    filesize = ftell( MareaFile );
    fseek( MareaFile, 0, SEEK_SET );

// check for valid MAREA_ID tag at the beginning of the file
    if( !fread( &fileid, sizeof(long), 1, MareaFile ) )
    {
        printf( "Unable to read fileid from '%s' - aborting.\n", MareaFilename );
        fclose( MareaFile );
        return -2;
    }
    if( fileid != MAREA_ID )
    {
        printf( "Incorrect MAREA_ID - expected %lX, found %lX - aborting.\n", MAREA_ID, fileid );
        fclose( MareaFile );
        return -3;
    }

// loop through marea.dat to read each message area
    while( !feof(MareaFile) && !ferror(MareaFile) && filesize > ftell( MareaFile ) )
    {
        num_marea++;
        marea = realloc( marea, num_marea * sizeof(MAREA) );
        if( !marea )
        {
            printf( "error - can't allocate %ld bytes for marea - aborting.\n", ((long)num_marea) * sizeof(MAREA) );
            fclose( MareaFile );
            return -4;
        }
        (*marea_ptr) = marea;

        if( !fread( &marea[num_marea-1], sizeof(MAREA), 1, MareaFile ) )
        {
            printf( "Unable to read marea #%d from '%s' - aborting.\n", num_marea, MareaFilename );
            fclose( MareaFile );
            return -5;
        }
        
        // read OVERRIDE structures for this area from file, and ignore them
        for( i=0; i<marea[num_marea-1].num_override; i++ )
        {
            if( !fread( &ovr, sizeof(ovr), 1, MareaFile ) )
            {
                printf( "Unable to read ovr from '%s' - aborting.\n", MareaFilename );
                fclose( MareaFile );
                return -6;
            }
        }

        // realloc *marea_heap_ptr to hold another ptr to this heap
        (*marea_heap_ptr) = realloc( (*marea_heap_ptr), num_marea * sizeof(unsigned char *) );
        if( !(*marea_heap_ptr) )
        {
            printf( "Unable to allocate %d bytes for *marea_heap_ptr - aborting.\n", num_marea * sizeof(unsigned char *) );
            fclose( MareaFile );
            return -8;
        }
        (*marea_heap_ptr)[num_marea-1] = NULL;
        
        if( marea[num_marea-1].cbHeap > 0 )
        {            
            // allocate heap for this marea
            heap = calloc( marea[num_marea-1].cbHeap, 1 );
            if( !heap )
            {
                printf( "calloc() error - can't allocate %d bytes - aborting.\n", marea[num_marea-1].cbHeap );
                fclose( MareaFile );
                return -7;
            }
            (*marea_heap_ptr)[num_marea-1] = heap;
            
            if( !fread( heap, marea[num_marea-1].cbHeap, 1, MareaFile ) )
            {
                printf( "Unable to read heap (%d bytes) from '%s' - aborting.\n", marea[num_marea-1].cbHeap, MareaFilename );
                fclose( MareaFile );
                return -8;
            }
        }
        else
        {
            printf( "warning - marea #%d - heap size 0??\n", num_marea );
        }
    }
    
// close marea.dat file    
    fclose( MareaFile );


    return num_marea;
}


// returns number of items created in MailUsersPtr array
// NOTE - *MailUsersPtr must be free()'d by caller after use, ReadMailUserCfg() will alloc memory but does not free() it
int ReadMailUserCfg( struct _user **MailUsersPtr, char *UsersCfgFilename )
{
    FILE *UsersCfgFile;
    char token[FILENAME_MAX];    
#define LINE_LEN    1024
    char line[LINE_LEN];
    char *lineptr;
    struct _user *MailUsers = NULL;
    long NumMailUsers = 0;
    

    if( !MailUsersPtr || !UsersCfgFilename )
    {
        printf( "error - NULL pointer passed to ReadMailUserCfg() - aborting.\n" );
        return -1;
    }    

    MailUsers = (*MailUsersPtr);

// open mailusers.cfg    
    UsersCfgFile = fopen( UsersCfgFilename, "rt" );
    if( !UsersCfgFile )
    {
        printf( "Error - unable to open %s - aborting.\n", UsersCfgFilename );
        return -1;
    }


    while( !feof(UsersCfgFile) && !ferror(UsersCfgFile) )
    {
        lineptr = fgets( line, LINE_LEN, UsersCfgFile );
        if( feof(UsersCfgFile) || ferror(UsersCfgFile) )
            break;
       
        lineptr = gettoken( lineptr, token, sizeof(token) );
        if( !lineptr )
            break;
            
        if( !stricmp( token, "user" ) )
        {
            // got 'user' token, rest of line is username to look for messages addressed to
            NumMailUsers++;
            (*MailUsersPtr) = realloc( (*MailUsersPtr), NumMailUsers * sizeof(struct _user) );
            if( !(*MailUsersPtr) )
            {
                printf( "error - can't realloc() %lu bytes for %lu users - stopping.\n", NumMailUsers * sizeof(struct _user), NumMailUsers );
                fclose( UsersCfgFile );
                return 2;
            }
            MailUsers = (*MailUsersPtr);
            memset( &MailUsers[NumMailUsers-1], '\0', sizeof(struct _user) );
            strncpy( MailUsers[NumMailUsers-1].username, trimwhitespace(lineptr), USERNAME_MAX );
            MailUsers[NumMailUsers-1].username[USERNAME_MAX-1] = '\0';
        }
        else if( !stricmp( token, "email" ) )
        {
            // got 'email' token, rest of line is email address to send notifications to
            if( NumMailUsers < 1 )
            {
                printf( "warning - can't handle 'email' before 'user' - skipping.\n" );
                break;          
            }
            strncpy( MailUsers[NumMailUsers-1].email, trimwhitespace(lineptr), EMAIL_MAX );
            MailUsers[NumMailUsers-1].email[EMAIL_MAX-1] = '\0';            
        }
        else if( !stricmp( token, "unsubscribed" ) )
        {
            // got 'unsubscribed' token, read int value following keyword to determine status
            if( NumMailUsers < 1 )
            {
                printf( "warning - can't handle 'unsubscribed' before 'user' - skipping.\n" );
                break;          
            }
            MailUsers[NumMailUsers-1].unsubscribed = strtol( lineptr, NULL, 0 );
        }
    }    
    
    
// close mailusers.cfg    
    fclose( UsersCfgFile );
    
        
    return NumMailUsers;
}


//NOTE - *marea_lastprocessed_ptr will be allocated inside read_marea_lastprocessed() and must later be free()'d by the caller
int read_marea_lastprocessed( struct _marea_lastprocessed **marea_lastprocessed_ptr, char *MailmindIdxFilename )
{
    struct _marea_lastprocessed *marea_lastprocessed = NULL;
    int num_marea = 0;
    FILE *MailmindIdxFile;
    long filesize;


    if( !marea_lastprocessed_ptr || !MailmindIdxFilename )
    {
        printf( "error - NULL pointer passed to read_marea_lastprocessed() - aborting.\n" );
        return -1;
    }

    (*marea_lastprocessed_ptr) = NULL;
    
// open "mailmind.idx"
    MailmindIdxFile = fopen( MailmindIdxFilename, "rb" );
    if( !MailmindIdxFile )
    {
        printf( "warning - can't open '%s' - new marea_lastprocessed file will be created.\n", MailmindIdxFilename );
        return 0;
    }


    fseek( MailmindIdxFile, 0, SEEK_END );
    filesize = ftell( MailmindIdxFile );
    fseek( MailmindIdxFile, 0, SEEK_SET );
    
    marea_lastprocessed = calloc( filesize, 1 );
    if( !marea_lastprocessed )
    {
        printf( "error - can't calloc() %l bytes - aborting.\n", filesize );
        fclose( MailmindIdxFile );
        return -2;
    }
    (*marea_lastprocessed_ptr) = marea_lastprocessed;   // set return pointer to marea_lastprocessed array of structs read from file
    
    if( fread( marea_lastprocessed, filesize, 1, MailmindIdxFile ) < 1 )
    {
        printf( "error - can't read %l bytes from '%s' - aborting.\n", filesize, MailmindIdxFilename );
        fclose( MailmindIdxFile );
        return -3;
    }
    
    num_marea = filesize / sizeof(struct _marea_lastprocessed);
    

// close "mailmind.idx"    
    fclose( MailmindIdxFile );
    

    return num_marea;
}


// NOTE - caller must free *usr after return (it will be allocated inside read_userbbs() but is not free()'d here)
int read_userbbs( struct _usr **usr, char *UserBbsFilename )
{
    FILE *UserBbsFile;
    long filesize;
    int NumUserBbs;
        
    
    if( !usr || !UserBbsFilename )
    {
        printf( "error - NULL pointer passed to read_userbbs() - aborting.\n" );
        return -1;
    }
    
    UserBbsFile = fopen( UserBbsFilename, "rb" );
    if( !UserBbsFile )
    {
        printf( "error - can't open user.bbs file '%s' - aborting.\n", UserBbsFilename );
        return -2;
    }
    
    
    fseek( UserBbsFile, 0, SEEK_END );
    filesize = ftell( UserBbsFile );
    fseek( UserBbsFile, 0, SEEK_SET );
    
    NumUserBbs = filesize / sizeof(struct _usr);
    
    (*usr) = calloc( filesize, 1 );
    if( !(*usr) )
    {
        printf( "error - can't allocate %ld bytes for struct _usr *usr - aborting.\n", filesize );
        return -3;
    }


    if( !fread( *usr, filesize, 1, UserBbsFile ) )
    {
        printf( "error - unable to read %ld bytes from user.bbs file - aborting.\n", filesize );
        fclose( UserBbsFile );
        return -4;
    }
        
    
    fclose( UserBbsFile );
    
    
    return NumUserBbs;
}



int main(int argc, char **argv)
{
    char UsersCfgFilename[FILENAME_MAX] = "d:\\bbs\\mailmind\\mailuser.cfg";
    struct _user *MailUsers = NULL;
    int NumMailUsers;
    char MailmindIdxFilename[FILENAME_MAX] = "d:\\bbs\\mailmind\\mailmind.idx";    
    struct _marea_lastprocessed *marea_lastprocessed = NULL;
    int num_marea_lastprocessed = 0;
    struct m_pointers prm;          // struct to hold max.prm (will be filled in by read_prm())
    unsigned char *prm_heap=NULL;   // heap for strings in max.prm (will be allocated and filled in by read_prm())
    char PrmFilename[FILENAME_MAX] = "d:\\bbs\\max\\max.prm";
    int retval;
    int NumUserBbs = 0;
    struct _usr *usr = NULL;        // this will be an array of struct _usr for entries in user.bbs
    char UserBbsFilename[FILENAME_MAX] = "";
    MAREA *marea = NULL;
    unsigned char **marea_heap_ptr = NULL;
    int num_marea = 0;
    char MareaFilename[FILENAME_MAX] = "";
    int i;


    printf( "MAILMIND v0.01 - Chris Gerlinsky, 2021\n" );


//TODO - create semaphore while mailmind is running?
//       problems will occur if another copy runs at the same time (duplicate notifications / possibly corrupt MAILMIND.IDX


// fill in MailUsers[] with details of usernames and email addresses from mailuser.cfg
    NumMailUsers = ReadMailUserCfg( &MailUsers, UsersCfgFilename );
//    printf( "NumMailUsers: %d\n", NumMailUsers );


// fill in marea_lastprocessed[] with array of structs with area tags and last read UMSGID
    num_marea_lastprocessed = read_marea_lastprocessed( &marea_lastprocessed, MailmindIdxFilename );
//    printf( "num_marea_lastprocessed: %d\n", num_marea_lastprocessed );


// get path and filename of marea.dat/idx files from max.prm
//TODO - check commandline to see if max.prm is specified
//TODO - if not specified on commandline, check environment variable MAX for path to directory containing max.prm
    retval = read_prm( &prm, &prm_heap, PrmFilename );
    if( retval )
    {
        printf( "error processing max.prm (%d).\n", retval );
    }


// open user.bbs file
    if( strlen(prm_heap+prm.user_file) + 5 < sizeof(UserBbsFilename) )        
        sprintf( UserBbsFilename, "%s.bbs", prm_heap+prm.user_file );
    else
        printf( "error - prm.user_file too long! '%s'\n", prm_heap+prm.user_file );
    NumUserBbs = read_userbbs( &usr, UserBbsFilename );

//    printf( "# of entries in user.bbs: %d\n", NumUserBbs );


// get list of message areas from marea.dat/idx files
// for any areas in marea.dat/idx that aren't in marea_lastprocessed, add them
    if( strlen(prm_heap+prm.marea_name) + 5 < sizeof(MareaFilename) )        
        sprintf( MareaFilename, "%s.dat", prm_heap+prm.marea_name );
    else
        printf( "error - prm.marea_name too long! '%s'\n", prm_heap+prm.marea_name );
    num_marea = read_marea( &marea, &marea_heap_ptr, MareaFilename );


// go through each message area, open area, search for messages addressed to anybody in MailUsers[]
// -> also check if user has access to the message area
// search begins at UMSGID + 1 (struct _marea_lastprocessed contains the last read UMSGID)
// if any matches found, send email notification to MailUsers[n].email

    retval = search_for_new_mail( marea, marea_heap_ptr, num_marea, usr, NumUserBbs, MailUsers, NumMailUsers, marea_lastprocessed, num_marea_lastprocessed, MailmindIdxFilename );


// clean up before exiting
    // free prm_heap if it was allocated by read_prm()    
    if( prm_heap )
        free(prm_heap);

    // free marea_lastprocessed if it was allocated by read_marea_lastprocessed()    
    if( marea_lastprocessed )
        free(marea_lastprocessed);
        
    // free MailUsers if it was allocated by ReadMailUserCfg()
    if( MailUsers )
        free(MailUsers);

    // free usr if it was allocated by read_userbbs()
    if( usr )
        free(usr);

    // free marea and marea_heap_ptr if they were allocated by read_marea
    if( marea )
        free(marea);

    if( marea_heap_ptr )
    {
        for( i=0; i<num_marea; i++ )
        {
            if( marea_heap_ptr[i] )
                free(marea_heap_ptr[i]);    
        }
    }


    return 0;
}
