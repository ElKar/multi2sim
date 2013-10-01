/*
 *  Multi2Sim
 *  Copyright (C) 2013  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <ext/stdio_filebuf.h>
#include <cassert>
#include <iomanip>
#include <sstream>

#include <arch/common/Asm.h>
#include <lib/cpp/Misc.h>

#include "Asm.h"
#include "Inst.h"


using namespace Misc;
using namespace SI;
using namespace std;


StringMap inst_format_map =
{
	{ "<invalid>", InstFormatInvalid },
	{ "sop2", InstFormatSOP2 },
	{ "sopk", InstFormatSOPK },
	{ "sop1", InstFormatSOP1 },
	{ "sopc", InstFormatSOPC },
	{ "sopp", InstFormatSOPP },
	{ "smrd", InstFormatSMRD },
	{ "vop2", InstFormatVOP2 },
	{ "vop1", InstFormatVOP1 },
	{ "vopc", InstFormatVOPC },
	{ "vop3a", InstFormatVOP3a },
	{ "vop3b", InstFormatVOP3b },
	{ "vintrp", InstFormatVINTRP },
	{ "ds", InstFormatDS },
	{ "mubuf", InstFormatMUBUF },
	{ "mtbuf", InstFormatMTBUF },
	{ "mimg", InstFormatMIMG },
	{ "exp", InstFormatEXP },
	{ 0, 0 }
};

/* String maps for assembly dump. */
StringMap inst_sdst_map =
{
	{"reserved", 0},
	{"reserved", 1},
	{"vcc_lo", 2},
	{"vcc_hi", 3},
	{"tba_lo", 4},
	{"tba_hi", 5},
	{"tma_lo", 6},
	{"tma_hi", 7},
	{"ttmp0", 8},
	{"ttmp1", 9},
	{"ttmp2", 10},
	{"ttmp3", 11},
	{"ttmp4", 12},
	{"ttmp5", 13},
	{"ttmp6", 14},
	{"ttmp7", 15},
	{"ttmp8", 16},
	{"ttmp9", 17},
	{"ttmp10", 18},
	{"ttmp11", 19},
	{"m0", 20},
	{"reserved", 21},
	{"exec_lo", 22},
	{"exec_hi", 23},
	{ 0, 0 }
};

StringMap inst_ssrc_map =
{
	{"0.5", 0},
	{"-0.5", 1},
	{"1.0", 2},
	{"-1.0", 3},
	{"2.0", 4},
	{"-2.0", 5},
	{"4.0", 6},
	{"-4.0", 7},
	{"reserved", 8},
	{"reserved", 9},
	{"reserved", 10},
	{"vccz", 11},
	{"execz", 12},
	{"scc", 13},
	{"reserved", 14},
	{"literal constant", 15},
	{0, 0}
};

StringMap inst_buf_data_format_map =
{
	{"invalid", InstBufDataFormatInvalid },
	{"BUF_DATA_FORMAT_8", InstBufDataFormat8 },
	{"BUF_DATA_FORMAT_16", InstBufDataFormat16 },
	{"BUF_DATA_FORMAT_8_8", InstBufDataFormat8_8 },
	{"BUF_DATA_FORMAT_32", InstBufDataFormat32 },
	{"BUF_DATA_FORMAT_16_16", InstBufDataFormat16_16 },
	{"BUF_DATA_FORMAT_10_11_11", InstBufDataFormat10_11_11 },
	{"BUF_DATA_FORMAT_11_10_10", InstBufDataFormat11_10_10 },
	{"BUF_DATA_FORMAT_10_10_10_2", InstBufDataFormat10_10_10_2 },
	{"BUF_DATA_FORMAT_2_10_10_10", InstBufDataFormat2_10_10_10 },
	{"BUF_DATA_FORMAT_8_8_8_8", InstBufDataFormat8_8_8_8 },
	{"BUF_DATA_FORMAT_32_32", InstBufDataFormat32_32 },
	{"BUF_DATA_FORMAT_16_16_16_16", InstBufDataFormat16_16_16_16 },
	{"BUF_DATA_FORMAT_32_32_32", InstBufDataFormat32_32_32 },
	{"BUF_DATA_FORMAT_32_32_32_32", InstBufDataFormat32_32_32_32 },
	{"reserved", InstBufDataFormatReserved },
	{ 0, 0 }
};

StringMap inst_buf_num_format_map =
{
	{"BUF_NUM_FORMAT_UNORM", InstBufNumFormatUnorm },
	{"BUF_NUM_FORMAT_SNORM", InstBufNumFormatSnorm },
	{"BUF_NUM_FORMAT_UNSCALED", InstBufNumFormatUnscaled },
	{"BUF_NUM_FORMAT_SSCALED", InstBufNumFormatSscaled },
	{"BUF_NUM_FORMAT_UINT", InstBufNumFormatUint },
	{"BUF_NUM_FORMAT_SINT", InstBufNumFormatSint },
	{"BUF_NUM_FORMAT_SNORM_NZ", InstBufNumFormatSnormNz },
	{"BUF_NUM_FORMAT_FLOAT", InstBufNumFormatFloat },
	{"reserved", InstBufNumFormatReserved },
	{"BUF_NUM_FORMAT_SRGB", InstBufNumFormatSrgb },
	{"BUF_NUM_FORMAT_UBNORM", InstBufNumFormatUbnorm },
	{"BUF_NUM_FORMAT_UBNORM_NZ", InstBufNumFormatUbnormNz },
	{"BUF_NUM_FORMAT_UBINT", InstBufNumFormatUbint },
	{"BUF_NUM_FORMAT_UBSCALED", InstBufNumFormatUbscaled },
	{ 0, 0 }
};

StringMap inst_OP16_map =
{
	{"f", 0},
	{"lt", 1},
	{"eq", 2},
	{"le", 3},
	{"gt", 4},
	{"lg", 5},
	{"ge", 6},
	{"o", 7},
	{"u", 8},
	{"nge", 9},
	{"nlg", 10},
	{"ngt", 11},
	{"nle", 12},
	{"neq", 13},
	{"nlt", 14},
	{"tru", 15},
	{ 0, 0 }
};

StringMap inst_OP8_map =
{
	{"f", 0},
	{"lt", 1},
	{"eq", 2},
	{"le", 3},
	{"gt", 4},
	{"lg", 5},
	{"ge", 6},
	{"tru", 7},
	{ 0, 0 }
};

StringMap inst_special_reg_map =
{
	{ "vcc", InstSpecialRegVcc },
	{ "scc", InstSpecialRegScc },
	{ "exec", InstSpecialRegExec },
	{ "tma", InstSpecialRegTma },
	{ 0, 0 }
};


void Inst::DumpOperand(ostream& os, int operand)
{
	assert(operand >= 0 && operand <= 511);
	if (operand <= 103)
	{
		/* SGPR */
		os << "s" << operand;
	}
	else if (operand <= 127)
	{
		/* sdst special registers */
		os << StringMapValue(inst_sdst_map, operand - 104);
	}
	else if (operand <= 192)
	{
		/* Positive integer constant */
		os << operand - 128;
	}
	else if (operand <= 208)
	{
		/* Negative integer constant */
		os << '-' << operand - 192;
	}
	else if (operand <= 239)
	{
		fatal("%s: operand code unused (%d)",
				__FUNCTION__, operand);
	}
	else if (operand <= 255)
	{
		os << StringMapValue(inst_ssrc_map, operand - 240);
	}
	else if (operand <= 511)
	{
		/* VGPR */
		os << "v" << operand - 256;
	}
}


void Inst::DumpOperandSeries(ostream& os, int start, int end)
{
	assert(start <= end);
	if (start == end)
	{
		DumpOperand(os, start);
		return;
	}

	if (start <= 103)
	{
		os << "s[" << start << ':' << end << ']';
	}
	else if (start <= 245)
	{
		if (start >= 112 && start <= 123)
		{
			assert(end <= 123);
			os << "ttmp[" << start - 112 << ':' << end - 112 << ']';
		}
		else
		{
			assert(end == start + 1);
			switch (start)
			{
			case 106:
				os << "vcc";
				break;
			case 108:
				os << "tba";
				break;
			case 110:
				os << "tma";
				break;
			case 126:
				os << "exec";
				break;
			case 128:
				os << "0";
				break;
			case 131:
				os << "3";
				break;
			case 208: 
				os << "-16";
				break;
			case 240:
				os << "0.5";
				break;
			case 242:
				os << "1.0";
				break;
			case 243:
				os << "-1.0";
				break;
			case 244:
				os << "2.0";
				break;
			case 245:
				os << "-2.0";
				break;
			default:
				fatal("Unimplemented series: "
					"[%d:%d]", start, end);
			}
		}
	}
	else if (start <= 255)
	{
		fatal("%s: illegal operand series: [%d:%d]",
				__FUNCTION__, start, end);
	}
	else if (start <= 511)
	{
		cout << "v[" << start - 256 << ':' << end - 256 << ']';
	}
}


void Inst::DumpScalar(ostream& os, int operand)
{
	DumpOperand(os, operand);
}


void Inst::DumpScalarSeries(ostream& os, int start, int end)
{
	DumpOperandSeries(os, start, end);
}


void Inst::DumpVector(ostream& os, int operand)
{
	DumpOperand(os, operand + 256);
}


void Inst::DumpVectorSeries(ostream& os, int start, int end)
{
	DumpOperandSeries(os, start + 256, end + 256);
}


void Inst::DumpOperandExp(ostream& os, int operand)
{
	assert(operand >= 0 && operand <= 63);
	if (operand <= 7)
	{
		/* EXP_MRT */
		os << "exp_mrt_" << operand;
	}
	else if (operand == 8)
	{
		/* EXP_Z */
		os << "exp_mrtz";
	}
	else if (operand == 9)
	{
		/* EXP_NULL */
		os << "exp_null";
	}
	else if (operand < 12)
	{
		fatal("%s: operand code [%d] unused.",
				__FUNCTION__, operand);
	}
	else if (operand <= 15)
	{
		/* EXP_POS */
		os << "exp_pos_" << operand - 12;
	}
	else if (operand < 32)
	{
		fatal("%s: operand code [%d] unused.",
				__FUNCTION__, operand);
	}
	else if (operand <= 63)
	{
		/* EXP_PARAM */
		os << "exp_prm_" << operand - 32;
	}
}


void Inst::DumpSeriesVdata(ostream& os, unsigned int vdata, int op)
{
	int vdata_end;

	switch (op)
	{
		case 0:
		case 4:
		case 9:
		case 12:
		case 24:
		case 28:
		case 50:
			vdata_end = vdata + 0;
			break;
		case 1:
		case 5:
			vdata_end = vdata + 1;
			break;
		case 2:
		case 6:
			vdata_end = vdata + 2;
			break;
		case 3:
		case 7:
			vdata_end = vdata + 3;
			break;
		default:
			fatal("MUBUF/MTBUF opcode not recognized");
	}

	DumpVectorSeries(os, vdata, vdata_end);
}


void Inst::DumpSsrc(ostream& os, unsigned int ssrc) 
{
	if (ssrc == 0xff)
		os << "0x" << setw(8) << setfill('0') << hex
				<< bytes.sop2.lit_cnst
				<< setfill(' ') << dec;
	else
		DumpScalar(os, ssrc);
}


void Inst::Dump64Ssrc(ostream& os, unsigned int ssrc)
{		
	if (ssrc == 0xff)
		os << "0x" << setw(8) << setfill('0') << hex
				<< bytes.sop2.lit_cnst
				<< setfill(' ') << dec;
	else
		DumpScalarSeries(os, ssrc, ssrc + 1);
}


void Inst::DumpVop3Src(std::ostream& os, unsigned int src, int neg)
{
	stringstream ss;

	DumpOperand(ss, src);
	if (!(InRange(bytes.vop3a.op, 293, 298)) && 
		!(InRange(bytes.vop3a.op, 365, 366)))
	{
		if ((bytes.vop3a.neg & neg) && 
				(bytes.vop3a.abs & neg))
			os << "-abs(" << ss.str() << ")";
		else if ((bytes.vop3a.neg & neg) && 
				!(bytes.vop3a.abs & neg))
			os << '-' << ss.str();
		else if (!(bytes.vop3a.neg & neg) && 
				(bytes.vop3a.abs & neg))
			os << "abs(" << ss.str() << ')';
		else if (!(bytes.vop3a.neg & neg) && 
				!(bytes.vop3a.abs & neg))
			os << ss.str();
	}
	else
	{
		if (bytes.vop3a.neg & neg)
			os << '-' << ss.str();
		else if (!(bytes.vop3a.neg & neg))
			os << ss.str();
	}
}


void Inst::DumpVop364Src(ostream& os, unsigned int src, int neg)
{
	stringstream ss;

	DumpOperandSeries(ss, src, src + 1);
	if (!(InRange(bytes.vop3a.op, 293, 298)) && 
		!(InRange(bytes.vop3a.op, 365, 366)))
	{
		if ((bytes.vop3a.neg & neg) && 
				(bytes.vop3a.abs & neg))
			os << "-abs(" << ss.str() << ')';
		else if ((bytes.vop3a.neg & neg) && 
				!(bytes.vop3a.abs & neg))
			os << "-" << ss.str();
		else if (!(bytes.vop3a.neg & neg) && 
				(bytes.vop3a.abs & neg))
			os << "abs(" << ss.str() << ")";
		else if (!(bytes.vop3a.neg & neg) && 
				!(bytes.vop3a.abs & neg))
			os << ss.str();
	}
	else
	{
		if (bytes.vop3a.neg & neg)
			os << '-' << ss.str();
		else if (!(bytes.vop3a.neg & neg))
			os << ss.str();
	}
}


void Inst::DumpMaddr(ostream& os)
{
	/* soffset */
	assert(bytes.mtbuf.soffset <= 103 ||
			bytes.mtbuf.soffset == 124 ||
			(bytes.mtbuf.soffset >= 128 && 
			bytes.mtbuf.soffset <= 208));
	DumpScalar(os, bytes.mtbuf.soffset);

	/* offen */
	if (bytes.mtbuf.offen)
		os << " offen";

	/* index */
	if (bytes.mtbuf.idxen)
		os << " idxen";

	/* offset */
	if (bytes.mtbuf.offset)
		os << " offset:" << bytes.mtbuf.offset;
}


void Inst::DumpDug(ostream& os)
{
	/* DMASK */
	os << " dmask:0x" << hex << bytes.mimg.dmask << dec;
	
	/* UNORM */
	if (bytes.mimg.unorm)
		os << " unorm";
	
	/* GLC */
	if (bytes.mimg.glc)
		os << " glc";
}


Inst::Inst(Asm *as)
{
	/* Initialize */
	this->as = as;
	Clear();
}


void Inst::Dump(ostream& os)
{
	int token_len;
	const char *fmt_str;
	
	/* Traverse format string */
	fmt_str = info ? info->fmt_str : "";
	while (*fmt_str)
	{
		/* Literal */
		if (*fmt_str != '%')
		{
			os << *fmt_str;
			fmt_str++;
			continue;
		}

		/* Token */
		fmt_str++;
		if (Common::Asm::IsToken(fmt_str, "WAIT_CNT", &token_len))
		{	
			InstBytesSOPP *sopp = &bytes.sopp;

			unsigned int more = 0;
			int vm_cnt = (sopp->simm16 & 0xF);

			if (vm_cnt != 0xF)
			{
				os << "vmcnt(" << vm_cnt << ")";
				more = 1;
			}

			int lgkm_cnt = (sopp->simm16 & 0x1f00) >> 8;
			if (lgkm_cnt != 0x1f)
			{
				if (more)
					os << " & ";
				os << "lgkmcnt(" << lgkm_cnt << ")";
				more = 1;
			}

			int exp_cnt = (sopp->simm16 & 0x70) >> 4;
			if (exp_cnt != 0x7)
			{
				if (more)
					os << " & ";
				os << "expcnt(" << exp_cnt << ")";
				more = 1;
			}
		}
		else if (Common::Asm::IsToken(fmt_str, "LABEL", &token_len))
		{		
			InstBytesSOPP *sopp = &bytes.sopp;
	
			short simm16 = sopp->simm16;
			int se_simm = simm16;

			os << "label_" << setw(4) << setfill('0') << hex
					<< uppercase << (address + (se_simm * 4) + 4) / 4
					<< dec << setfill(' ') << nouppercase;
		}
		else if (Common::Asm::IsToken(fmt_str, "SSRC0", &token_len))
		{	
			DumpSsrc(os, bytes.sop2.ssrc0);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_SSRC0", &token_len))
		{
			Dump64Ssrc(os, bytes.sop2.ssrc0);
		}
		else if (Common::Asm::IsToken(fmt_str, "SSRC1", &token_len))
		{
			DumpSsrc(os, bytes.sop2.ssrc1);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_SSRC1", &token_len))
		{
			Dump64Ssrc(os, bytes.sop2.ssrc1);
		}
		else if (Common::Asm::IsToken(fmt_str, "SDST", &token_len))
		{	
			DumpScalar(os, bytes.sop2.sdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_SDST", &token_len))
		{
			DumpScalarSeries(os, bytes.sop2.sdst, bytes.sop2.sdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "SIMM16", &token_len))
		{
			os << "0x" << setw(4) << setfill('0') << hex
					<< bytes.sopk.simm16
					<< dec << setfill(' ');
		}
		else if (Common::Asm::IsToken(fmt_str, "SRC0", &token_len))
		{
			if (bytes.vopc.src0 == 0xFF)
				os << "0x" << setw(8) << setfill('0') << hex
						<< bytes.vopc.lit_cnst
						<< dec << setfill(' ');
			else
				DumpOperand(os, bytes.vopc.src0);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_SRC0", &token_len))
		{
			assert(bytes.vopc.src0 != 0xFF);
			DumpOperandSeries(os, bytes.vopc.src0, bytes.vopc.src0 + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VSRC1", &token_len))
		{
			DumpVector(os, bytes.vopc.vsrc1);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_VSRC1", &token_len))
		{
			assert(bytes.vopc.vsrc1 != 0xFF);
			DumpVectorSeries(os, bytes.vopc.vsrc1, bytes.vopc.vsrc1 + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VDST", &token_len))
		{
			DumpVector(os, bytes.vop1.vdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "64_VDST", &token_len))
		{
			DumpVectorSeries(os, bytes.vop1.vdst, bytes.vop1.vdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "SVDST", &token_len))
		{
			DumpScalar(os, bytes.vop1.vdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_SVDST", &token_len))
		{
			/* VOP3a compare operations use the VDST field to 
			 * indicate the address of the scalar destination.*/
			DumpScalarSeries(os, bytes.vop3a.vdst, bytes.vop3a.vdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_VDST", &token_len))
		{
			DumpVector(os, bytes.vop3a.vdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_VDST", &token_len))
		{
			DumpVectorSeries(os, bytes.vop3a.vdst, bytes.vop3a.vdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_SDST", &token_len))
		{
			DumpScalarSeries(os, bytes.vop3b.sdst, bytes.vop3b.sdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_SRC0", &token_len))
		{
			DumpVop3Src(os, bytes.vop3a.src0, 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_SRC0", &token_len))
		{
			DumpVop364Src(os, bytes.vop3a.src0, 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_SRC1", &token_len))
		{
			DumpVop3Src(os, bytes.vop3a.src1, 2);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_SRC1", &token_len))
		{
			DumpVop364Src(os, bytes.vop3a.src1, 2);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_SRC2", &token_len))
		{
			DumpVop3Src(os, bytes.vop3a.src2, 4);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_64_SRC2", &token_len))
		{
			DumpVop364Src(os, bytes.vop3a.src2, 4);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_OP16", &token_len))
		{
			os << StringMapValue(inst_OP16_map, bytes.vop3a.op & 15);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP3_OP8", &token_len))
		{
			os << StringMapValue(inst_OP8_map, bytes.vop3a.op & 15);
		}
		else if (Common::Asm::IsToken(fmt_str, "SMRD_SDST", &token_len))
		{
			DumpScalar(os, bytes.smrd.sdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "SERIES_SDST", &token_len))
		{
			/* The sbase field is missing the LSB, 
			 * so multiply by 2 */
			int sdst = bytes.smrd.sdst;
			int sdst_end;
			int op = bytes.smrd.op;

			/* S_LOAD_DWORD */
			if (InRange(op, 0, 4))
			{
				/* Multi-dword */
				switch (op)
				{
				case 0:
					break;
				case 1:
					sdst_end = sdst + 1;
					break;
				case 2:
					sdst_end = sdst + 3;
					break;
				case 3:
					sdst_end = sdst + 7;
					break;
				case 4:
					sdst_end = sdst + 15;
					break;
				default:
					fatal("Invalid smrd "
						"opcode");
				}
			}
			/* S_BUFFER_LOAD_DWORD */
			else if (InRange(op, 8, 12))
			{	
				/* Multi-dword */
				switch (op)
				{
				case 8:
					break;
				case 9:
					sdst_end = sdst + 1;
					break;
				case 10:
					sdst_end = sdst + 3;
					break;
				case 11:
					sdst_end = sdst + 7;
					break;
				case 12:
					sdst_end = sdst + 15;
					break;
				default:
					fatal("Invalid smrd "
						"opcode");
				}
			}
			/* S_MEMTIME */
			else if (op == 30)
			{
				fatal("S_MEMTIME instruction not currently" 
					"supported");
			}
			/* S_DCACHE_INV */
			else if (op == 31)
			{
				fatal("S_DCACHE_INV instruction not" 
					"currently supported");
			}
			else
			{
				fatal("Invalid smrd opcode");
			}

			DumpScalarSeries(os, sdst, sdst_end);

		}
		else if (Common::Asm::IsToken(fmt_str, "SERIES_SBASE", &token_len))
		{
			
			/* The sbase field is missing the LSB, 
			 * so multiply by 2 */
			int sbase = bytes.smrd.sbase * 2;
			int sbase_end;
			int op = bytes.smrd.op;

			/* S_LOAD_DWORD */
			if (InRange(op, 0, 4))
			{
				/* SBASE specifies two consecutive SGPRs */
				sbase_end = sbase + 1;
			}
			/* S_BUFFER_LOAD_DWORD */
			else if (InRange(op, 8, 12))
			{
				/* SBASE specifies four consecutive SGPRs */
				sbase_end = sbase + 3;
			}
			/* S_MEMTIME */
			else if (op == 30)
			{
				fatal("S_MEMTIME instruction not currently"
					" supported");
			}
			/* S_DCACHE_INV */
			else if (op == 31)
			{
				fatal("S_DCACHE_INV instruction not currently"
					" supported");
			}
			else
			{
				fatal("Invalid smrd opcode");
			}

			DumpScalarSeries(os, sbase, sbase_end);
		}
		else if (Common::Asm::IsToken(fmt_str, "VOP2_LIT", &token_len))
		{
			os << "0x" << setw(8) << setfill('0') << hex
					<< bytes.vop2.lit_cnst
					<< setfill(' ') << dec;
		}
		else if (Common::Asm::IsToken(fmt_str, "OFFSET", &token_len))
		{
			if (bytes.smrd.imm)
				os << "0x" << setw(2) << setfill('0') << hex
						<< bytes.smrd.offset
						<< setfill(' ') << dec;
			else
				DumpScalar(os, bytes.smrd.offset);
		}
		else if (Common::Asm::IsToken(fmt_str, "DS_VDST", &token_len))
		{
			DumpVector(os, bytes.ds.vdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "ADDR", &token_len))
		{
			DumpVector(os, bytes.ds.addr);
		}
		else if (Common::Asm::IsToken(fmt_str, "DATA0", &token_len))
		{
			DumpVector(os, bytes.ds.data0);
		}
		else if (Common::Asm::IsToken(fmt_str, "DATA1", &token_len))
		{
			DumpVector(os, bytes.ds.data1);
		}
		else if (Common::Asm::IsToken(fmt_str, "OFFSET0", &token_len))
		{
			if (bytes.ds.offset0)
				os << "offset0:" << bytes.ds.offset0 << ' ';
		}
		else if (Common::Asm::IsToken(fmt_str, "DS_SERIES_VDST", &token_len))
		{
			DumpVectorSeries(os, bytes.ds.vdst, bytes.ds.vdst + 1);
		}
		else if (Common::Asm::IsToken(fmt_str, "OFFSET1", &token_len))
		{
			if (bytes.ds.offset1)
				os << "offset1:" << bytes.ds.offset1 << ' ';
		}
		else if (Common::Asm::IsToken(fmt_str, "VINTRP_VDST", &token_len))
		{
			DumpVector(os, bytes.vintrp.vdst);
		}
		else if (Common::Asm::IsToken(fmt_str, "VSRC_I_J", &token_len))
		{
			DumpVector(os, bytes.vintrp.vsrc);
		}
		else if (Common::Asm::IsToken(fmt_str, "ATTR", &token_len))
		{
			os << "attr_" << bytes.vintrp.attr;
		}
		else if (Common::Asm::IsToken(fmt_str, "ATTRCHAN", &token_len))
		{
			switch (bytes.vintrp.attrchan)
			{
			case 0:
				os << 'x';
				break;
			case 1:
				os << 'y';
				break;
			case 2:
				os << 'z';
				break;
			case 3:
				os << 'w';
				break;
			default:
				break;
			}
		}
		else if (Common::Asm::IsToken(fmt_str, "MU_SERIES_VDATA", &token_len))
		{
			DumpSeriesVdata(os, bytes.mubuf.vdata, bytes.mubuf.op);
		}
		else if (Common::Asm::IsToken(fmt_str, "MU_GLC", &token_len))
		{
			if (bytes.mubuf.glc)
				os << "glc";
		}
		else if (Common::Asm::IsToken(fmt_str, "VADDR", &token_len))
		{
			if (bytes.mtbuf.offen && bytes.mtbuf.idxen)
				DumpVectorSeries(os, bytes.mtbuf.vaddr, 
						bytes.mtbuf.vaddr + 1);
			else
				DumpVector(os, bytes.mtbuf.vaddr);
		}
		else if (Common::Asm::IsToken(fmt_str, "MU_MADDR", &token_len))
		{
			DumpMaddr(os);
		}
		else if (Common::Asm::IsToken(fmt_str, "MT_SERIES_VDATA", &token_len))
		{
			DumpSeriesVdata(os, bytes.mtbuf.vdata, bytes.mtbuf.op);
		}
		else if (Common::Asm::IsToken(fmt_str, "SERIES_SRSRC", &token_len))
		{
			assert((bytes.mtbuf.srsrc << 2) % 4 == 0);
			DumpScalarSeries(os, bytes.mtbuf.srsrc << 2, 
					(bytes.mtbuf.srsrc << 2) + 3);
		}
		else if (Common::Asm::IsToken(fmt_str, "MT_MADDR", &token_len))
		{
			DumpMaddr(os);
			os << " format:["
					<< StringMapValue(inst_buf_data_format_map, 
					bytes.mtbuf.dfmt) << ','
					<< StringMapValue(inst_buf_num_format_map, 
					bytes.mtbuf.nfmt) << ']';
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_SERIES_VDATA", &token_len))
		{
			DumpVectorSeries(os, bytes.mimg.vdata,
					bytes.mimg.vdata + 3);
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_VADDR", &token_len))
		{
			DumpVectorSeries(os, bytes.mimg.vaddr, 
					bytes.mimg.vaddr + 3);
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_SERIES_SRSRC", &token_len))
		{
			assert((bytes.mimg.srsrc << 2) % 4 == 0);
			DumpScalarSeries(os, bytes.mimg.srsrc << 2, 
					(bytes.mimg.srsrc << 2) + 7);
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_DUG_SERIES_SRSRC", &token_len))
		{
			assert((bytes.mimg.srsrc << 2) % 4 == 0);
			DumpScalarSeries(os, bytes.mimg.srsrc << 2, 
					(bytes.mimg.srsrc << 2) + 7);
			DumpDug(os);
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_SERIES_SSAMP", &token_len))
		{
			assert((bytes.mimg.ssamp << 2) % 4 == 0);
			DumpScalarSeries(os, bytes.mimg.ssamp << 2, 
					(bytes.mimg.ssamp << 2) + 3);
		}
		else if (Common::Asm::IsToken(fmt_str, "MIMG_DUG_SERIES_SSAMP", 
			&token_len))
		{
			assert((bytes.mimg.ssamp << 2) % 4 == 0);
			DumpScalarSeries(os, bytes.mimg.ssamp << 2, 
					(bytes.mimg.ssamp << 2) + 3);
			DumpDug(os);
		}
		else if (Common::Asm::IsToken(fmt_str, "TGT", &token_len))
		{
			DumpOperandExp(os, bytes.exp.tgt);
		}
		else if (Common::Asm::IsToken(fmt_str, "EXP_VSRCs", &token_len))
		{
			if (bytes.exp.compr == 0 && 
					(bytes.exp.en && 0x0) == 0x0)
			{
				os << '[';
				DumpVector(os, bytes.exp.vsrc0);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc1);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc2);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc3);
				os << ']';
			}
			else if (bytes.exp.compr == 1 && 
				(bytes.exp.en && 0x0) == 0x0)
			{
				os << '[';
				DumpVector(os, bytes.exp.vsrc0);
				os << ' ';
				DumpVector(os, bytes.exp.vsrc1);
				os << ']';
			}
		}
		else
		{
			fatal("%s: token not recognized.", fmt_str);
		}

		fmt_str += token_len;
	}
}


void Inst::Clear()
{
	info = NULL;
	bytes.dword = 0;
	size = 0;
	address = 0;
}


void Inst::Decode(char *buf, unsigned int address)
{
	/* Initialize instruction */
	info = NULL;
	size = 4;
	bytes.word[0] = * (unsigned int *) buf;
	bytes.word[1] = 0;
	this->address = address;

	/* Use the encoding field to determine the instruction type */
	if (bytes.sopp.enc == 0x17F)
	{
		if (!as->GetDecTableSopp(bytes.sopp.op))
		{
			fatal("Unimplemented Instruction: SOPP:%d  "
				"// %08X: %08X\n", bytes.sopp.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableSopp(bytes.sopp.op);
	}
	else if (bytes.sopc.enc == 0x17E)
	{
		if (!as->GetDecTableSopc(bytes.sopc.op))
		{
			fatal("Unimplemented Instruction: SOPC:%d  "
				"// %08X: %08X\n", bytes.sopc.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableSopc(bytes.sopc.op);

		/* Only one source field may use a literal constant,
		 * which is indicated by 0xFF. */
		assert(!(bytes.sopc.ssrc0 == 0xFF &&
			bytes.sopc.ssrc1 == 0xFF));
		if (bytes.sopc.ssrc0 == 0xFF ||
			bytes.sopc.ssrc1 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.sop1.enc == 0x17D)
	{
		if (!as->GetDecTableSop1(bytes.sop1.op))
		{
			fatal("Unimplemented Instruction: SOP1:%d  "
				"// %08X: %08X\n", bytes.sop1.op,
				address, *(unsigned int*)buf);
		}

		info = as->GetDecTableSop1(bytes.sop1.op);

		/* 0xFF indicates the use of a literal constant as a
		 * source operand. */
		if (bytes.sop1.ssrc0 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.sopk.enc == 0xB)
	{
		if (!as->GetDecTableSopk(bytes.sopk.op))
		{
			fatal("Unimplemented Instruction: SOPK:%d  "
				"// %08X: %08X\n", bytes.sopk.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableSopk(bytes.sopk.op);
	}
	else if (bytes.sop2.enc == 0x2)
	{
		if (!as->GetDecTableSop2(bytes.sop2.op))
		{
			fatal("Unimplemented Instruction: SOP2:%d  "
				"// %08X: %08X\n", bytes.sop2.op,
				address, *(unsigned int *)buf);
		}

		info = as->GetDecTableSop2(bytes.sop2.op);

		/* Only one source field may use a literal constant,
		 * which is indicated by 0xFF. */
		assert(!(bytes.sop2.ssrc0 == 0xFF &&
			bytes.sop2.ssrc1 == 0xFF));
		if (bytes.sop2.ssrc0 == 0xFF ||
			bytes.sop2.ssrc1 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.smrd.enc == 0x18)
	{
		if (!as->GetDecTableSmrd(bytes.smrd.op))
		{
			fatal("Unimplemented Instruction: SMRD:%d  "
				"// %08X: %08X\n", bytes.smrd.op,
				address, *(unsigned int *)buf);
		}

		info = as->GetDecTableSmrd(bytes.smrd.op);
	}
	else if (bytes.vop3a.enc == 0x34)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;

		if (!as->GetDecTableVop3(bytes.vop3a.op))
		{
			fatal("Unimplemented Instruction: VOP3:%d  "
				"// %08X: %08X %08X\n",
				bytes.vop3a.op, address,
				*(unsigned int *)buf,
				*(unsigned int *)(buf + 4));
		}

		info = as->GetDecTableVop3(bytes.vop3a.op);
	}
	else if (bytes.vopc.enc == 0x3E)
	{
		if (!as->GetDecTableVopc(bytes.vopc.op))
		{
			fatal("Unimplemented Instruction: VOPC:%d  "
				"// %08X: %08X\n",
				bytes.vopc.op, address,
				*(unsigned int *)buf);
		}

		info = as->GetDecTableVopc(bytes.vopc.op);

		/* 0xFF indicates the use of a literal constant as a
		 * source operand. */
		if (bytes.vopc.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.vop1.enc == 0x3F)
	{
		if (!as->GetDecTableVop1(bytes.vop1.op))
		{
			fatal("Unimplemented Instruction: VOP1:%d  "
				"// %08X: %08X\n", bytes.vop1.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableVop1(bytes.vop1.op);

		/* 0xFF indicates the use of a literal constant as a
		 * source operand. */
		if (bytes.vop1.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.vop2.enc == 0x0)
	{
		if (!as->GetDecTableVop2(bytes.vop2.op))
		{
			fatal("Unimplemented Instruction: VOP2:%d  "
				"// %08X: %08X\n", bytes.vop2.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableVop2(bytes.vop2.op);

		/* 0xFF indicates the use of a literal constant as a
		 * source operand. */
		if (bytes.vop2.src0 == 0xFF)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}

		/* Some opcodes define a 32-bit literal constant following
		 * the instruction */
		if (bytes.vop2.op == 32)
		{
			size = 8;
			bytes.dword = * (unsigned long long *) buf;
		}
	}
	else if (bytes.vintrp.enc == 0x32)
	{
		if (!as->GetDecTableVintrp(bytes.vintrp.op))
		{
			fatal("Unimplemented Instruction: VINTRP:%d  "
				"// %08X: %08X\n", bytes.vintrp.op,
				address, * (unsigned int *) buf);
		}

		info = as->GetDecTableVintrp(bytes.vintrp.op);

	}
	else if (bytes.ds.enc == 0x36)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;
		if (!as->GetDecTableDs(bytes.ds.op))
		{
			fatal("Unimplemented Instruction: DS:%d  "
				"// %08X: %08X %08X\n", bytes.ds.op,
				address, *(unsigned int *)buf,
				*(unsigned int *)(buf + 4));
		}

		info = as->GetDecTableDs(bytes.ds.op);
	}
	else if (bytes.mtbuf.enc == 0x3A)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;

		if (!as->GetDecTableMtbuf(bytes.mtbuf.op))
		{
			fatal("Unimplemented Instruction: MTBUF:%d  "
				"// %08X: %08X %08X\n",
				bytes.mtbuf.op, address,
				*(unsigned int *)buf, *(unsigned int *)(buf+4));
		}

		info = as->GetDecTableMtbuf(bytes.mtbuf.op);
	}
	else if (bytes.mubuf.enc == 0x38)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;

		if (!as->GetDecTableMubuf(bytes.mubuf.op))
		{
			fatal("Unimplemented Instruction: MUBUF:%d  "
				"// %08X: %08X %08X\n",
				bytes.mubuf.op, address,
				*(unsigned int *)buf,
				*(unsigned int *)(buf+4));
		}

		info = as->GetDecTableMubuf(bytes.mubuf.op);
	}
	else if (bytes.mimg.enc == 0x3C)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;

		if(!as->GetDecTableMimg(bytes.mimg.op))
		{
			fatal("Unimplemented Instruction: MIMG:%d  "
				"// %08X: %08X %08X\n",
				bytes.mimg.op, address,
				*(unsigned int *)buf,
				*(unsigned int *)(buf+4));
		}

		info = as->GetDecTableMimg(bytes.mimg.op);
	}
	else if (bytes.exp.enc == 0x3E)
	{
		/* 64 bit instruction. */
		size = 8;
		bytes.dword = * (unsigned long long *) buf;

		/* Export is the only instruction in its kind */
		if (!as->GetDecTableExp(0))
			fatal("Unimplemented Instruction: EXP\n");

		info = as->GetDecTableExp(0);
	}
	else
	{
		fatal("Unimplemented format. Instruction is:  // %08X: %08X\n",
				address, ((unsigned int*)buf)[0]);
	}
}
	
	
const char *Inst::SpecialRegToString(InstSpecialReg value)
{
	return StringMapValue(inst_special_reg_map, value);
}


const char *Inst::BufDataFormatToString(InstBufDataFormat value)
{
	return StringMapValue(inst_buf_data_format_map, value);
}


const char *Inst::BufNumFormatToString(InstBufNumFormat value)
{
	return StringMapValue(inst_buf_num_format_map, value);
}


const char *Inst::FormatToString(InstFormat value)
{
	return StringMapValue(inst_format_map, value);
}
	

InstSpecialReg Inst::StringToSpecialReg(const char *text)
{
	return (InstSpecialReg) StringMapString(inst_special_reg_map, text);
}



/*
 * C Wrapper
 */


struct SIInstWrap *SIInstWrapCreate(SIAsmWrap *as)
{
	Inst *inst = new Inst((Asm *) as);
	return (SIInstWrap *) inst;
}


void SIInstWrapFree(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	delete inst;
}


void SIInstWrapDecode(struct SIInstWrap *self, char *buffer, unsigned int offset)
{
	Inst *inst = (Inst *) self;
	inst->Decode(buffer, offset);
}


void SIInstWrapDump(struct SIInstWrap *self, FILE *f)
{
	Inst *inst = (Inst *) self;
	__gnu_cxx::stdio_filebuf<char> filebuf(fileno(f), std::ios::out);
	ostream os(&filebuf);
	inst->Dump(os);
}


void SIInstWrapDumpBuf(struct SIInstWrap *self, char *buffer, int size)
{
	stringstream ss;
	Inst *inst = (Inst *) self;
	inst->Dump(ss);
	snprintf(buffer, size, "%s", ss.str().c_str());
}


void SIInstWrapClear(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	inst->Clear();
}


int SIInstWrapGetOp(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return inst->GetOp();
}


SIInstOpcode SIInstWrapGetOpcode(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return (SIInstOpcode) inst->GetOpcode();
}


SIInstBytes *SIInstWrapGetBytes(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return (SIInstBytes *) inst->GetBytes();
}


const char *SIInstWrapGetName(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return inst->GetName();
}


SIInstFormat SIInstWrapGetFormat(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return (SIInstFormat) inst->GetFormat();
}


int SIInstWrapGetSize(struct SIInstWrap *self)
{
	Inst *inst = (Inst *) self;
	return inst->GetSize();
}


const char *SIInstWrapSpecialRegToString(SIInstSpecialReg value)
{
	return Inst::SpecialRegToString((InstSpecialReg) value);
}


const char *SIInstWrapBufDataFormatToString(SIInstBufDataFormat value)
{
	return Inst::BufDataFormatToString((InstBufDataFormat) value);
}


const char *SIInstWrapBufNumFormatToString(SIInstBufNumFormat value)
{
	return Inst::BufNumFormatToString((InstBufNumFormat) value);
}


const char *SIInstWrapFormatToString(SIInstFormat value)
{
	return Inst::FormatToString((InstFormat) value);
}


SIInstSpecialReg SIInstWrapStringToSpecialReg(const char *text)
{
	return (SIInstSpecialReg) Inst::StringToSpecialReg(text);
}


SIInstBufDataFormat SIInstWrapStringToBufDataFormat(const char *text, int *err_ptr)
{
	SIInstBufDataFormat result;
	bool err;

	result = (SIInstBufDataFormat) StringMapString(inst_buf_data_format_map, text, err);
	if (err_ptr)
		*err_ptr = err;
	return result;
}


SIInstBufNumFormat SIInstWrapStringToBufNumFormat(const char *text, int *err_ptr)
{
	SIInstBufNumFormat result;
	bool err;

	result = (SIInstBufNumFormat) StringMapString(inst_buf_num_format_map, text, err);
	if (err_ptr)
		*err_ptr = err;
	return result;
}
