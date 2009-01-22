#include "PrecompiledHeader.h"

#include "Common.h"
#include "iR5900.h"
#include "InterTables.h"


namespace R5900 {
namespace Dynarec {
namespace OpcodeImpl {

////////////////////////////////////////////////////
//static void recCACHE( void ) {
//	MOV32ItoM( (uptr)&cpuRegs.code, cpuRegs.code );
//	MOV32ItoM( (uptr)&cpuRegs.pc, pc );
//	iFlushCall(FLUSH_EVERYTHING);
//	CALLFunc( (uptr)CACHE );
//	//branch = 2;
//
//	CMP32ItoM((int)&cpuRegs.pc, pc);
//	j8Ptr[0] = JE8(0);
//	RET();
//	x86SetJ8(j8Ptr[0]);
//}


void recPREF( void ) 
{
}

void recSYNC( void )
{
}

void recMFSA( void ) 
{
	int mmreg;
	if (!_Rd_) return;

	mmreg = _checkXMMreg(XMMTYPE_GPRREG, _Rd_, MODE_WRITE);
	if( mmreg >= 0 ) {
		SSE_MOVLPS_M64_to_XMM(mmreg, (uptr)&cpuRegs.sa);
	}
	else if( (mmreg = _checkMMXreg(MMX_GPR+_Rd_, MODE_WRITE)) >= 0 ) {
		MOVDMtoMMX(mmreg, (uptr)&cpuRegs.sa);
		SetMMXstate();
	}
	else {
		MOV32MtoR(EAX, (u32)&cpuRegs.sa);
		_deleteEEreg(_Rd_, 0);
		MOV32RtoM((uptr)&cpuRegs.GPR.r[_Rd_].UL[0], EAX);
		MOV32ItoM((uptr)&cpuRegs.GPR.r[_Rd_].UL[1], 0);
	}
}

void recMTSA( void )
{
	if( GPR_IS_CONST1(_Rs_) ) {
		MOV32ItoM((uptr)&cpuRegs.sa, g_cpuConstRegs[_Rs_].UL[0] );
	}
	else {
		int mmreg;
		
		if( (mmreg = _checkXMMreg(XMMTYPE_GPRREG, _Rs_, MODE_READ)) >= 0 ) {
			SSE_MOVSS_XMM_to_M32((uptr)&cpuRegs.sa, mmreg);
		}
		else if( (mmreg = _checkMMXreg(MMX_GPR+_Rs_, MODE_READ)) >= 0 ) {
			MOVDMMXtoM((uptr)&cpuRegs.sa, mmreg);
			SetMMXstate();
		}
		else {
			MOV32MtoR(EAX, (uptr)&cpuRegs.GPR.r[_Rs_].UL[0]);
			MOV32RtoM((uptr)&cpuRegs.sa, EAX);
		}
	}
}

void recMTSAB( void ) 
{
	if( GPR_IS_CONST1(_Rs_) ) {
		MOV32ItoM((uptr)&cpuRegs.sa, ((g_cpuConstRegs[_Rs_].UL[0] & 0xF) ^ (_Imm_ & 0xF)) << 3);
	}
	else {
		_eeMoveGPRtoR(EAX, _Rs_);
		AND32ItoR(EAX, 0xF);
		XOR32ItoR(EAX, _Imm_&0xf);
		SHL32ItoR(EAX, 3);
		MOV32RtoM((uptr)&cpuRegs.sa, EAX);
	}
}

void recMTSAH( void ) 
{
	if( GPR_IS_CONST1(_Rs_) ) {
		MOV32ItoM((uptr)&cpuRegs.sa, ((g_cpuConstRegs[_Rs_].UL[0] & 0x7) ^ (_Imm_ & 0x7)) << 4);
	}
	else {
		_eeMoveGPRtoR(EAX, _Rs_);
		AND32ItoR(EAX, 0x7);
		XOR32ItoR(EAX, _Imm_&0x7);
		SHL32ItoR(EAX, 4);
		MOV32RtoM((uptr)&cpuRegs.sa, EAX);
	}
}

	////////////////////////////////////////////////////
	void recNULL( void )
	{
		Console::Error("EE: Unimplemented op %x", params cpuRegs.code);
	}

	////////////////////////////////////////////////////
	void recUnknown()
	{
		// TODO : Unknown ops should throw an exception.
		Console::Error("EE: Unrecognized op %x", params cpuRegs.code);
	}

	void recMMI_Unknown()
	{
		// TODO : Unknown ops should throw an exception.
		Console::Error("EE: Unrecognized MMI op %x", params cpuRegs.code);
	}

	void recCOP0_Unknown()
	{
		// TODO : Unknown ops should throw an exception.
		Console::Error("EE: Unrecognized COP0 op %x", params cpuRegs.code);
	}

	void recCOP1_Unknown()
	{
		// TODO : Unknown ops should throw an exception.
		Console::Error("EE: Unrecognized FPU/COP1 op %x", params cpuRegs.code);
	}

	/**********************************************************
	*    UNHANDLED YET OPCODES
	*
	**********************************************************/

	void recCACHE()
	{
	   MOV32ItoM( (uptr)&cpuRegs.code, (u32)cpuRegs.code );
	   MOV32ItoM( (uptr)&cpuRegs.pc, (u32)pc );
	   iFlushCall(FLUSH_EVERYTHING);
	   CALLFunc( (uptr)R5900::Interpreter::OpcodeImpl::CACHE );
	   branch = 2;
	}

	void recTGE( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TGE );
	}

	void recTGEU( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TGEU );
	}

	void recTLT( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TLT );
	}

	void recTLTU( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TLTU );
	}

	void recTEQ( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TEQ );
	}

	void recTNE( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TNE );
	}

	void recTGEI( void )
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TGEI );
	}

	void recTGEIU( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TGEIU );
	}

	void recTLTI( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TLTI );
	}

	void recTLTIU( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TLTIU );
	}

	void recTEQI( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TEQI );
	}

	void recTNEI( void ) 
	{
		recBranchCall( R5900::Interpreter::OpcodeImpl::TNEI );
	}

} }}		// end Namespace R5900::Dynarec::OpcodeImpl