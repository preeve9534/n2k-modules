/**********************************************************************
 * GroupFunctionHandlers.cpp
 * Copyright (c) 2021 Paul Reeve <preeve@pdjr.eu>
 * 
 * This file implements a number of classes providing group function
 * support for PGNs 128006, 128007 and 128008.
 * 
 * GroupFunctionHandlerForPGN128006 implements a Command function
 * handler which allows a remote to update any field.
 * 
 * GroupFunctionHandlerForPGN128007 implements a Request function
 * handler which allows a remote to request transmission of the
 * eponymous PGN. Note that requests for individual field values are
 * not honoured - any request results in transmission of a complete
 * PGN
 */ 

#include <string.h>
#include <NMEA2000StdTypes.h>
#include "GroupFunctionHandlers.h"
#include "NMEA2000.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

///////////////////////////////////////////////////////////////////////
// START OF HANDLERS FOR PGN128006
///////////////////////////////////////////////////////////////////////

bool GroupFunctionHandlerForPGN128006::HandleRequest(const tN2kMsg &N2kMsg, uint32_t TransmissionInterval, uint16_t TransmissionIntervalOffset, uint8_t  NumberOfParameterPairs, int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec = GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
  bool MatchFilter=true;
  tN2kMsg N2kRMsg;
  uint8_t Instance=0xff;

  // Start to build response
  SetStartAcknowledge(
    N2kRMsg,
    N2kMsg.Source,
    PGN,
    N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
    pec,
    NumberOfParameterPairs
  );
  N2kRMsg.Destination=N2kMsg.Source;

  if (NumberOfParameterPairs > 0) {
    int i;
    int index;
    unsigned char field;
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField = false;

    StartParseRequestPairParameters(N2kMsg, index);
    for (i = 0; (i < NumberOfParameterPairs) && (MatchFilter || tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if (!FoundInvalidField) {
        field = N2kMsg.GetByte(index);
        switch (field) {
          case 2:
            break;
          case 3:
            break;
          case 4:
            break;
          case 5:
            break;
          case 6:
            break;
          case 7:
            break;
          case 8:
            break;
          case 9:
            break;
          default:
            FieldErrorCode = N2kgfpec_InvalidRequestOrCommandParameterField;
            MatchFilter = false;
            FoundInvalidField = true;
            break;
        }
      } else {
        FieldErrorCode = N2kgfpec_TemporarilyUnableToComply;
      }
      AddAcknowledgeParameter(N2kRMsg, i, FieldErrorCode);
    }
  }
}

bool GroupFunctionHandlerForPGN128006::HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev) {
  int i;
  int Index;
  uint8_t field;
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec = N2kgfTPec_Acknowledge;
  tN2kGroupFunctionParameterErrorCode PARec;
  tN2kMsg N2kRMsg;
  int canUpdate = true;
  PGN128006_Field fields[] = { {false,0},{false,0},{false,0},{false,0},{false,0},{false,0},{false,0},{false,0},{false,0},{false,0} };

 	if (PrioritySetting != 0x08 || PrioritySetting != 0x0f || PrioritySetting != 0x09) pec = N2kgfTPec_TransmitIntervalOrPriorityNotSupported;

  SetStartAcknowledge(
    N2kRMsg,
    N2kMsg.Source,
    128006L,
    N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
    pec,
    NumberOfParameterPairs
  );

  StartParseCommandPairParameters(N2kMsg, Index);
  for (i = 0; i < NumberOfParameterPairs; i++) {
    field = N2kMsg.GetByte(Index);
    PARec = N2kgfpec_Acknowledge;
    switch (field) {
      case 2:
        fields[field].dirty = true;
        fields[field].value.F02 = N2kMsg.GetByte(Index);
        break;
      case 3:
        switch(N2kMsg.GetByte(Index) & 0x0f) {
          case 0: fields[field].dirty = true; fields[field].value.F03 = N2kDD473_OFF; break;
          case 1: fields[field].dirty = true; fields[field].value.F03 = N2kDD473_ThrusterReady; break;
          case 2: fields[field].dirty = true; fields[field].value.F03 = N2kDD473_ThrusterToPORT; break;
          case 3: fields[field].dirty = true; fields[field].value.F03 = N2kDD473_ThrusterToSTARBOARD; break;
          default: PARec = N2kgfpec_RequestOrCommandParameterOutOfRange; canUpdate = false; break;
        }
        break;
      case 4:
        switch(N2kMsg.GetByte(Index) & 0x03) {
          case 0: fields[field].dirty = true; fields[field].value.F04 = N2kDD002_Off; break;
          case 1: fields[field].dirty = true; fields[field].value.F04 = N2kDD002_On; break;
          case 2: fields[field].dirty = true; fields[field].value.F04 = N2kDD002_Error; break;
          case 3: fields[field].dirty = true; fields[field].value.F04 = N2kDD002_Unavailable; break;
          default: PARec = N2kgfpec_RequestOrCommandParameterOutOfRange; canUpdate = false; break;
        }
        break;
      case 5:
        switch(N2kMsg.GetByte(Index) & 0x03) {
          case 0: fields[field].dirty = true; fields[field].value.F05 = N2kDD474_OFF; break;
          case 1: fields[field].dirty = true; fields[field].value.F05 = N2kDD474_Extend; break;
          case 2: fields[field].dirty = true; fields[field].value.F05 = N2kDD474_Retract; break;
          default: PARec = N2kgfpec_RequestOrCommandParameterOutOfRange; canUpdate = false; break;
        }
        break;
      case 6:
        fields[field].dirty = true; fields[field].value.F06 = N2kMsg.GetByte(Index);
        break;
      case 7:
        fields[field].dirty = true; fields[field].value.F07.SetEvents(N2kMsg.GetByte(Index));
        break;
      case 8:
        fields[field].dirty = true; fields[field].value.F08 = N2kMsg.Get1ByteUDouble(0.005, Index);
        break;      
      case 9:
        fields[field].dirty = true; fields[field].value.F09 = N2kMsg.Get2ByteDouble(0.0001, Index);
        break;
      default:
        PARec = N2kgfpec_InvalidRequestOrCommandParameterField;
    }
    AddAcknowledgeParameter(N2kRMsg, i, PARec);
  }
  pNMEA2000->SendMsg(N2kRMsg, iDev);

  if (canUpdate) this->updateFunction(fields);

  return true;
}

///////////////////////////////////////////////////////////////////////
// END OF HANDLERS FOR PGN128006
///////////////////////////////////////////////////////////////////////

#endif