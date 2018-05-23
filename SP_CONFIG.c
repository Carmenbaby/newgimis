#include "newgimis.h"
#include "newgimis.brh"
#include "AEEMenu.h"
#include "AEEText.h"




/*======================================================================= 
Function: DistToSemi()

Description: 
   Utility function that determines index of the first semicolon in the
   input string.

Prototype:

   int DistToSemi(const char * pszStr);

Parameters:
   pszStr: [in]. String to check.

Return Value:

   The index of the first semicolon.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
int DistToSemi(const char * pszStr)
{
   int nCount = 0;

   if ( !pszStr ) { 
      return -1;
   }

   while ( *pszStr != 0 ) {
      if ( *pszStr == ';' ) {
         return nCount;
      }
      else {
         nCount++;
         pszStr++;
      }
   }

   return -1;
}

/*======================================================================= 
Function: SamplePosDet_ReadGPSSettings()

Description: 
   Reads the GPS configuration settings from the configuration file.

Prototype:

   uint32 SamplePosDet_ReadGPSSettings(CSamplePosDet *pMe, IFile * pIFile);

Parameters:
   pMe: [in]. CSamplePosDet instance.
   pIFile: [in].  Pointer to the IFile instance representing the config
                  file.

Return Value:

  SUCCESS - If the settings were read successfully.
  Error code otherwise.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 testgps_ReadGPSSettings(newgimis *pMe, IFile * pIFile)
{
   char    *pszBuf = NULL;
   char    *pszTok = NULL;
   char    *pszSvr = NULL;
   char    *pszDelimiter = ";";
   int32   nResult = 0;
   FileInfo fiInfo;

   if ( SUCCESS != IFILE_GetInfo( pIFile, &fiInfo ) ) {
      return EFAILED;
   }

   if ( fiInfo.dwSize == 0 ) {
      return EFAILED;
   }

   // Allocate enough memory to read the full text into memory
   pszBuf = MALLOC( fiInfo.dwSize );

   nResult = IFILE_Read( pIFile, pszBuf, fiInfo.dwSize );
   if ( (uint32)nResult < fiInfo.dwSize ) {
      FREE( pszBuf );
      return EFAILED;
   }

   // Check for an optimization mode setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_OPT_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_OPT_STRING );
      pMe->gpsSettings.optim = (AEEGPSOpt)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a QoS setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_QOS_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_QOS_STRING );
      pMe->gpsSettings.qos = (AEEGPSQos)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a server type setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_TYPE_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_TYPE_STRING );
      pMe->gpsSettings.server.svrType = STRTOUL( pszTok, &pszDelimiter, 10 );

      // If the server type is IP, we need to find the ip address and the port number
      if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType ) {
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_IP_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_IP_STRING );
            nResult = DistToSemi( pszTok );
            pszSvr = MALLOC( nResult+1 );
            STRNCPY( pszSvr, pszTok, nResult );
            *(pszSvr+nResult) = 0;  // Need to manually NULL-terminate the string
            if ( !INET_ATON( pszSvr, &pMe->gpsSettings.server.svr.ipsvr.addr ) ) {
               FREE( pszBuf );
               FREE( pszSvr );
               return EFAILED;
            }
            FREE( pszSvr );
         }
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_PORT_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_PORT_STRING );
            pMe->gpsSettings.server.svr.ipsvr.port = AEE_htons((INPort)STRTOUL( pszTok, &pszDelimiter, 10 ));
         }
      }
   }


   FREE( pszBuf );
   
   return SUCCESS;
}



/*======================================================================= 
Function: SamplePosDet_WriteGPSSettings()

Description: 
   Write the GPS configuration settings from the configuration file.

Prototype:

   uint32 SamplePosDet_WriteGPSSettings(CSamplePosDet *pMe, IFile * pIFile);

Parameters:
   pMe: [in]. CSamplePosDet instance.
   pIFile: [in].  Pointer to the IFile instance representing the config
                  file.

Return Value:

  SUCCESS - If the settings were written successfully.
  EFAILED - If an error occured.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 testgps_WriteGPSSettings( newgimis *pMe, IFile * pIFile )
{
   char    *pszBuf;
   int32    nResult;

   pszBuf = MALLOC( 1024 );

   // Truncate the file, in case it already contains data
   IFILE_Truncate( pIFile, 0 );

   // Write out the optimization setting:
   SPRINTF( pszBuf, SPD_CONFIG_OPT_STRING"%d;\r\n", pMe->gpsSettings.optim );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   // Write out the QoS setting:
   SPRINTF( pszBuf, SPD_CONFIG_QOS_STRING"%d;\r\n", pMe->gpsSettings.qos );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   // Write out the server type setting:
   SPRINTF( pszBuf, SPD_CONFIG_SVR_TYPE_STRING"%d;\r\n", pMe->gpsSettings.server.svrType );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType )
   {
      // Write out the IP address setting:
      INET_NTOA( pMe->gpsSettings.server.svr.ipsvr.addr, pszBuf, 50 );
      nResult = IFILE_Write( pIFile, SPD_CONFIG_SVR_IP_STRING, STRLEN( SPD_CONFIG_SVR_IP_STRING ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
      nResult = IFILE_Write( pIFile, ";\r\n", STRLEN( ";\r\n" ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }

      // Write out the port setting:
      SPRINTF( pszBuf, SPD_CONFIG_SVR_PORT_STRING"%d;\r\n", AEE_ntohs(pMe->gpsSettings.server.svr.ipsvr.port) );
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
   }

   FREE( pszBuf );

   return SUCCESS;
}
