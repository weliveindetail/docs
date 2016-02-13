void MemoryBackendSSE::compileCallbackPoint(MemoryAddress pInsertionPoint, 
                                            CTDHandler *pHandler, 
                                            bool *pCallbackFlag)
{
  tByte *pCode = pInsertionPoint;

  #if SRS_DEBUG_BREAK_ON_ENTER_HANDLER
    // int 3
    pCode[0] = OPCODE_INT3;
    moveInsertionPoint(&pCode, 1);
  #endif

  // mov ebx, pCallbackFlag (5 byte)
  pCode[0] = OPCODE_MOV_CONST_TO_EBX;
  writeValue32(pCode + 1, pCallbackFlag);
  moveInsertionPoint(pCode, MEMORY_ADDRESS_SIZE + 1);

  // cmp byte ptr [ebx], 0x01 (3 byte)
  pCode[0] = OPCODE_CMP_BYTE;
  pCode[1] = 0x38 + PHYS_REG_CODE_EBX;
  pCode[2] = 0x01;
  moveInsertionPoint(pCode, 3);

  // jne [to the next instruction after the call] (2 byte)
  pCode[0] = OPCODE_NEAR_JUMP_IF_NOT_EQUAL;
  pCode[1] = getJumpOverCallbackPointDistance(pInsertionPoint, pCode);
  moveInsertionPoint(pCode, 2);

  // push ecx, push edx (2 byte)
  pCode[0] = OPCODE_PUSH_PHYS_REG + PHYS_REG_CODE_ECX;
  pCode[1] = OPCODE_PUSH_PHYS_REG + PHYS_REG_CODE_EDX;
  moveInsertionPoint(pCode, 2);

  // push nCurrentHandlerIndex (5 byte)
  pCode[0] = OPCODE_PUSH_CONST_DWORD;
  writeValue32(pCode + 1, pHandler);
  moveInsertionPoint(pCode, MEMORY_ADDRESS_SIZE + 1);

  // mov ebx, pCallee (5 byte)
  pCode[0] = OPCODE_MOV_CONST_TO_EBX;
  writeValue32(pCode + 1, onCallback);
  moveInsertionPoint(pCode, MEMORY_ADDRESS_SIZE + 1);

  // call ebx (2 byte)
  pCode[0] = OPCODE_ABSOLUTE_CALL; 
  pCode[1] = 0xD0 + PHYS_REG_CODE_EBX;
  moveInsertionPoint(pCode, 2);

  // pop edx, pop ecx (2 byte)
  pCode[0] = OPCODE_POP_PHYS_REG + PHYS_REG_CODE_EDX;
  pCode[1] = OPCODE_POP_PHYS_REG + PHYS_REG_CODE_ECX;
  moveInsertionPoint(pCode, 2);  

  M_SRS_ASSERT(pCode-pInsertionPoint == getCallbackPointSize());
}