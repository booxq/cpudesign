#include <cstdint>
#include <iostream>

using namespace std;

// Instructions identified by opcode
#define AUIPC 0x17   //将顶端立即数加至PC 
#define LUI 0x37     //加载顶端立即数 
#define JAL 0x6F     //无条件跳转 :rd,imm 
#define JALR 0x67    //有条件跳转 :rd,rsl,imm


// Branches using BRANCH as the label for common opcode
#define BRANCH 0x63  //分支指令 

#define BEQ 0x0      //分支指令 = 
#define BNE 0x1		 //分支指令 ≠ 
#define BLT 0x4      //分支指令 < 
#define BGE 0x5		 //分支指令 ≥ 
#define BLTU 0x6	 //分支指令 <无符号数 
#define BGEU 0x7     //分支指令 ≥无符号数 


// Loads using LOAD as the label for common opcode
#define LOAD 0x03    //加载指令 

#define LB 0x0       //加载Byte 
#define LH 0x1		 //加载半个字 
#define LW 0x2       //加载一个字 
#define LBU 0x4      //加载一个无符号Byte 
#define LHU 0x5      //加载半个无符号字 


// Stores using STORE as the label for common opcode
#define STORE 0x23   //存储命令 

#define SB 0x0       //存储一个Byte 
#define SH 0x1       //存储半个字 
#define SW 0x2       //存储一个字 


// ALU ops with one immediate
#define ALUIMM 0x13  //ALU中立即数指令集 

#define ADDI 0x0  //立即数相加 
#define SLTI 0x2  //set<立即数 
#define SLTIU 0x3 //set<无符号数 
#define XORI 0x4  //异或立即数 
#define ORI 0x6   //或立即数 
#define ANDI 0x7  //与立即数 
#define SLLI 0x1  //逻辑左移立即数 

#define SHR 0x5   // 右移集合，funct3：common funct3 for SRLI and SRAI

#define SRLI 0x0  //逻辑右移立即数 
#define SRAI 0x20 //算数右移立即数 


// ALU ops with all register operands
#define ALURRR 0x33  //ALU-寄存器 

#define ADDSUB 0x0  // common funct3 for ADD and SUB
#define ADD 0x0     //寄存器加 
#define SUB 0x20    //寄存器减 

#define SLL 0x1     //逻辑左移，寄存器 
#define SLT 0x2     //set<寄存器的值 
#define SLTU 0x3    //set<无符号寄存器的值 
#define XOR 0x4     //寄存器异或 
#define OR 0x6      //寄存器或 
#define AND 0x7     //寄存器与 

#define SRLA 0x5    // common funct3 for SRL and SRA，逻辑与算数右移 

#define SRL 0x0     //逻辑右移 
#define SRA 0x20    //算数右移 

// Fences using FENCES as the label for common opcode

#define FENCES 0x0F  //Synch
#define FENCE 0x0    //Synch thread
#define FENCE_I 0x1  //Synch Instr & Data

// CSR related instructions
#define CSRX 0x73

#define CALLBREAK 0x0  // common funct3 for ECALL and EBREAK
#define ECALL 0x0    //call指令 
#define EBREAK 0x1   //break中断指令 

#define CSRRW 0x1    //Read/Write
#define CSRRS 0x2    //Read & Set Bit
#define CSRRC 0x3    //Read & Clear Bit
#define CSRRWI 0x5   //Read/Write Imm
#define CSRRSI 0x6   //Read & Set Bit Imm
#define CSRRCI 0x7   //Read & Clear Bit Imm


// Data for memory
const int WORDSIZE = sizeof(uint32_t);
unsigned int MSize = 4096;
char* M;

// Functions for memory
int allocMem(uint32_t s) {
	M = new char[s];
	MSize = s;

	return s;
}

//释放内存 
void freeMem() {
	delete[] M;
	MSize = 0;
}

//读取比特 
char readByte(unsigned int address) {
	if(address >= MSize) {
		cout << "ERROR: Address out of range in readByte" << endl;
		return 0;
	}

	return M[address];
}

//写入比特 
void writeByte(unsigned int address, char data) {
	if(address >= MSize) {
		cout << "ERROR: Address out of range in writeByte" << endl;
		return;
	}

	M[address] = data;
}

//读取字 
uint32_t readWord(unsigned int address) {
	if(address >= MSize-WORDSIZE) {
		cout << "ERROR: Address out of range in readWord" << endl;
		return 0;
	}
	//取地址并进行强制类型转换 
	return *((uint32_t*)&(M[address]));
}

//读取半个字 
uint32_t readHalfWord(unsigned int address){
	if(address >= MSize-WORDSIZE/2) {
		cout << "ERROR: Address out of range in readWord" << endl;
		return 0;
	}

	return *((uint16_t*)&(M[address]));
}

//写入一个字 
void writeWord(unsigned int address, uint32_t data) {
	if(address >= MSize-WORDSIZE) {
		cout << "ERROR: Address out of range in writeWord" << endl;
		return;
	}

	*((uint32_t*)&(M[address])) = data;
}

//写半个字 
void writeHalfWord(unsigned int address, uint32_t data) {
	if(address >= MSize-WORDSIZE/2) {
		cout << "ERROR: Address out of range in writeWord" << endl;
		return;
	}

	*((uint16_t*)&(M[address])) = data;
}

// Write memory with instructions to test
void progMem() {
	// Write starts with PC at 0
	writeWord(0, (0xfffff << 12) | (2 << 7) | (LUI));  //imm,rd,opcode;这里是加载顶端立即数操作，内容是 0xfffff137 
	writeWord(4, (1 << 12) | (5 << 7) | (AUIPC));  //imm,rd,opcode;顶端立即数加至PC，0x00001297
	writeWord(8, (0x20<<25) | (5<<20) | (0<<15) | (SW << 12) | (0 << 7) | (STORE));//imm,rs2,rs1,funct3,imm,opcode;
	writeWord(12, (0x400<<20) | (0<<15) | (LB<<12) | (3<<7) | (LOAD));
	writeWord(16, (0x400<<20) | (0<<15) | (LBU<<12) | (7<<7) | (LOAD));
	writeWord(20, (0x0<<25) | (2<<20) | (0<<15) | (BGE<<12) | (0x8<<7) | (BRANCH));
	writeWord(28, (0x8<<20) | (3<<15) | (SLTIU<<12) | (8<<7) | (ALUIMM));
	writeWord(32, (SRAI<<25) | (0x2<<20) | (0x2<<15) | (SHR<<12) | (9<<7) | (ALUIMM));
	
	writeWord(36,(0x400)<<20|(1<<15)|(JALR<<12)|(4<<7)|(JALR));
	writeWord(40, (0x20<<25) | (7<<20) | (0<<15) | (SH << 12) | (9 << 7) | (STORE));
	writeWord(44, (0x0<<25) | (4<<20) | (1<<15) | (BGEU<<12) | (0x8<<7) | (BRANCH));
	writeWord(48, (0x400<<20) | (2<<15) | (ORI<<12) | (4<<7) | (ALUIMM));
	writeWord(52, (SUB<<25) | (4<<20) | (2<<15) | (SUB << 12) | (9 << 7) | (ALURRR));
	
	writeWord(56, (1<<31) |(0<<25) | (8<<20) | (0<<15) | (BLTU << 12) | (0 << 11) |(0 << 7) | (BRANCH));
	writeWord(60, (0x20<<25) | (8<<20) | (0<<15) | (SB << 12) | (0 << 7) | (STORE));
	writeWord(64, (0x100<<20) | (3<<15) | (XORI << 12) | (9 << 7) | (ALUIMM));
	writeWord(68, (ADD<<25) | (3<<20) | (1<<15) | (ADDSUB << 12) | (10 << 7) | (ALURRR));
	writeWord(72, (1 << 31) |(1 << 23) |(1 << 22) |(1 << 12) | (7 << 7) | (JAL));
	
	writeWord(0, 0x0013ab73);// CSRRS
	writeWord(4, 0x0013db73);//CSRRWI
	writeWord(8, 0x0013fb73);//CSRRCI
	writeWord(12, 0x0000100f);//FENCE_I
	writeWord(16, 0x00100073);//EBREAK
}

// ============================================================================


// data for CPU
uint32_t PC, NextPC;   //定义程序计数器和下一个程序计数器 
uint32_t R[32];   //定义32个无符号32位寄存器 
uint32_t IR;      //定义无符号32位指令寄存器 

unsigned int opcode;   //R\I\S\U类指令0-6位 
unsigned int rs1, rs2, rd;  //定义源寄存器(rs1\rs2)和目标寄存器(rd) 
unsigned int funct7, funct3;  //funct7是R类25-31位，funct3是R\I\S类12-14位 
unsigned int shamt;
unsigned int pred, succ;
unsigned int csr, zimm;  

// immediate values for I-type, S-type, B-type, U-type, J-type
unsigned int imm11_0i;   //I-type 
unsigned int imm11_5s, imm4_0s;    //S-type imm[11:5] immm[4:0] 
unsigned int imm12b, imm10_5b, imm4_1b, imm11b;  //B-type
unsigned int imm31_12u;    //U-type 
unsigned int imm20j, imm10_1j, imm11j, imm19_12j;  //J-type
//没有R-type，因为R-type:funct7 rs2 rs1 funct3 rd opcode 

unsigned int imm_temp;
unsigned int src1,src2;


unsigned int Imm11_0ItypeZeroExtended;
int Imm11_0ItypeSignExtended;
int Imm11_0StypeSignExtended;
unsigned int Imm12_1BtypeZeroExtended;
int Imm12_1BtypeSignExtended;
unsigned int Imm31_12UtypeZeroFilled;
int Imm20_1JtypeSignExtended;
int Imm20_1JtypeZeroExtended;

// Functions for CPU，CPU指令 
void decode(uint32_t instruction) {
	// Extract all bit fields from instruction
	opcode = instruction & 0x7F;   //因为opcode是7位，所以这里&0x7F 
	rd = (instruction & 0x0F80) >> 7;//rd为7-11位，&0x0F80,右移7位即可
	rs1 = (instruction & 0xF8000) >> 15;//rs1为15-19位，&0xF8000,右移15位即可
	zimm = rs1;
	rs2 = (instruction & 0x1F00000) >> 20; //rs2为20-24位，&0x1F00000,右移20位即可 
	shamt = rs2;
	funct3 = (instruction & 0x7000) >> 12;//funct3为12-14位，&0x7000,右移12位 
	funct7 = instruction >> 25;   //int类型，直接右移25位即可 
	imm11_0i = ((int32_t)instruction) >> 20;
	csr = instruction >> 20;
	imm11_5s = ((int32_t)instruction) >> 25;
	imm4_0s = (instruction >> 7) & 0x01F;
	imm12b = ((int32_t)instruction) >> 31;
	imm10_5b = (instruction >> 25) & 0x3F;
	imm4_1b = (instruction & 0x0F00) >> 8;
	imm11b = (instruction & 0x080) >> 7;
	imm31_12u = instruction >> 12;
	imm20j = ((int32_t)instruction) >> 31;
	imm10_1j = (instruction >> 21) & 0x3FF;
	imm11j = (instruction >> 20) & 1;
	imm19_12j = (instruction >> 12) & 0x0FF;
	pred = (instruction >> 24) & 0x0F;
	succ = (instruction >> 20) & 0x0F;

	// ========================================================================
	// Get values of rs1 and rs2
	src1 = R[rs1];
	src2 = R[rs2];

	// Immediate values
	Imm11_0ItypeZeroExtended = imm11_0i & 0x0FFF;
	Imm11_0ItypeSignExtended = imm11_0i;

	Imm11_0StypeSignExtended = (imm11_5s << 5) | imm4_0s;

	Imm12_1BtypeZeroExtended = imm12b & 0x00001000 | (imm11b << 11) | (imm10_5b << 5) | (imm4_1b << 1);
	Imm12_1BtypeSignExtended = imm12b & 0xFFFFF000 | (imm11b << 11) | (imm10_5b << 5) | (imm4_1b << 1);

	Imm31_12UtypeZeroFilled = instruction & 0xFFFFF000;

	Imm20_1JtypeSignExtended = (imm20j & 0xFFF00000) | (imm19_12j << 12) | (imm11j << 11) | (imm10_1j << 1);
	Imm20_1JtypeZeroExtended = (imm20j & 0x00100000) | (imm19_12j << 12) | (imm11j << 11) | (imm10_1j << 1);
	// ========================================================================
}

//打印寄存器内容 
void showRegs() {
	cout << "PC=0x" << std::hex << PC << " " << "IR=0x" << std::hex << IR << endl;

	for(int i=0; i<32; i++) {
		cout << "R[" << i << "]=0x" << std::hex << R[i] << " ";
	}
	
	cout << endl;
	
/*	for(int i=0; i<32; i++) {
		cout << "M[" << i << "]=0x" << std::hex << M[i] << " ";
	}
	
	cout << endl;*/
}

int main(int argc, char const *argv[]) {
	/* code */
	allocMem(4096); //开辟内存空间 
	progMem();  //编辑内存 

	PC = 0;

	char c = 'Y';

	while(c != 'n') {
		cout << "Registers bofore executing the instruction @0x" << std::hex << PC << endl;
		showRegs();  //显示寄存器 

		IR = readWord(PC);     //读取程序计数器中的内容，交给指令寄存器 
		NextPC = PC + WORDSIZE;   //下一个地址是PC+WORDSIZE决定的 

		decode(IR);  //对指令进行译码 

		switch(opcode) {   //判断 
			case LUI:
				cout << "Do LUI" << endl;
				R[rd] = Imm31_12UtypeZeroFilled;
				break;
			case AUIPC:
				cout << "Do AUIPC" << endl;
				cout << "PC = " << PC << endl;
				cout << "Imm31_12UtypeZeroFilled = " << Imm31_12UtypeZeroFilled << endl;
				R[rd] = PC + Imm31_12UtypeZeroFilled;
				break;
			case JAL:
				cout << "Do JAL" << endl;
				R[rd]=PC+4;
				NextPC = PC+ Imm20_1JtypeSignExtended;    
				break;
			case JALR:
				cout << "DO JALR" << endl;
				R[rd]=PC+4;
				NextPC=R[rs1]+Imm20_1JtypeSignExtended;
				break;
			case BRANCH:  //分支指令 
				switch(funct3) {
					case BEQ:
						cout << "DO BEQ" << endl;
						if(src1==src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BNE:
						cout << "Do BNE " << endl;
						if(src1!=src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BLT:
						cout << "Do BLT" << endl;
						if((int)src1<(int)src2){
							NextPC = PC + Imm12_1BtypeSignExtended;
						}
						break;
					case BGE:
						cout << "Do BGE" << endl;
						if((int)src1 >= (int)src2)
							NextPC = PC + Imm12_1BtypeSignExtended;
						break;
					case BLTU:
						cout << "Do BLTU" << endl;
						if(src1<src2){
							NextPC=PC+Imm12_1BtypeSignExtended;
						}
						break;
					case BGEU:
						cout<<"Do BGEU"<<endl;

						if(src1>=src2){
							NextPC=PC+Imm12_1BtypeSignExtended;
						}    
						break;
					default:
						cout << "ERROR: Unknown funct3 in BRANCH instruction " << IR << endl;
				}
				break;
			case LOAD:  //load指令 
				switch(funct3) {
					case LB:
						cout << "DO LB" << endl;
						unsigned int LB_LH,LB_LH_UP;
						cout << "LB Address is: " << src1+Imm11_0ItypeSignExtended << endl;
						LB_LH=readByte(src1+Imm11_0ItypeSignExtended);
						LB_LH_UP=LB_LH>>7;
						if(LB_LH_UP==1){
							//LB_LH=0xffffff00 & LB_LH;
							LB_LH=0xffffff00 | LB_LH;
						}else{
							LB_LH=0x000000ff & LB_LH;
						}
						R[rd]=LB_LH; 
						break;
					case LH:
						cout << "Do LH " << endl;
						unsigned int temp_LH,temp_LH_UP;
						temp_LH=readHalfWord(src1+Imm11_0ItypeSignExtended);
						temp_LH_UP=temp_LH>>15;
						if(temp_LH_UP==1){
							temp_LH=0xffff0000 | temp_LH;
						}else{
							temp_LH=0x0000ffff & temp_LH;
						}
						R[rd]=temp_LH; 
						break;
					case LW:
						cout << "Do LW" << endl;
						unsigned int temp_LW,temp_LW_UP;
						temp_LW=readByte(src1+Imm11_0ItypeSignExtended);
						temp_LW_UP=temp_LW>>31;
						if(temp_LW_UP==1){
							temp_LW=0x00000000 | temp_LW;
						}else{
							temp_LW=0xffffffff & temp_LW;
						}
						R[rd]=temp_LW;
						break;
					case LBU:
						cout << "Do LBU" << endl;
						R[rd] = readByte(Imm11_0ItypeSignExtended + src1) & 0x000000ff;
						break;
					case LHU:
						cout << "Do LHU" << endl;
						R[rd] = readByte(Imm11_0ItypeSignExtended + src1) & 0x0000ffff;
						break;
					default:
						cout << "ERROR: Unknown funct3 in LOAD instruction " << IR << endl;
				}
				break;
			case STORE:  //存储指令 
				switch(funct3) {
					case SB:
						cout << "Do SB" << endl;
						char sb_d1;
						unsigned int sb_a1;
						sb_d1=R[rs2] & 0xff;
						sb_a1 = R[rs1] +Imm11_0StypeSignExtended;
						writeByte(sb_a1, sb_d1);
						break;
					case SH:
						cout<<"Do SH"<<endl;
						uint16_t j;
						j=R[rs2]&0xffff;
						unsigned int x;
						x = R[rs1] + Imm11_0StypeSignExtended;
						writeHalfWord(x,j);
						break;
					case SW:
						cout << "DO SW" << endl;
						//unsigned int imm_temp;
						uint32_t _swData;
						_swData=R[rs2] & 0xffffffff;
						unsigned int _swR;
						_swR = R[rs1] + Imm11_0StypeSignExtended;
						cout << "SW Addr and Data are: " << _swR << ", " << _swData << endl;
						writeWord(_swR, _swData);
						break;
					default:
						cout << "ERROR: Unknown funct3 in STORE instruction " << IR << endl;
				}
				break;
			case ALUIMM:  //ALU立即数指令 
				switch(funct3) {
					case ADDI:
						cout <<    "Do ADDI" << endl;
						R[rd]=src1+Imm11_0ItypeSignExtended;
						break;
					case SLTI:
						cout << "Do SLTI" << endl;
						if(src1<Imm11_0ItypeSignExtended)
							R[rd] = 1;
						else
							R[rd] = 0;
						break;
					case SLTIU:
						cout << "Do SLTIU" << endl;
						if(src1<(unsigned int)Imm11_0ItypeSignExtended)
							R[rd] = 1;
						else
							R[rd] = 0;
						break;
					case XORI:
						cout << "Do XORI" << endl;
						R[rd]=(Imm11_0ItypeSignExtended)^R[rs1];
						break;
					case ORI:
						cout<<"Do ORI"<<endl;
						R[rd]=R[rs1]|Imm11_0ItypeSignExtended;
						break;
					case ANDI:
						cout << "DO ANDI"<<endl;
						R[rd]=R[rs1]&Imm11_0ItypeSignExtended;
						break;
					case SLLI:
						cout << "Do SLLI " << endl;
						R[rd]=src1<<shamt;
						break;
					case SHR:
						switch(funct7) {
							case SRLI:
								cout << "Do SRLI" << endl;
								R[rd]=src1>>shamt;
								break;
							case SRAI:
								cout << "Do SRAI" << endl;
								R[rd] = ((int)src1) >> shamt;
								break;
							default:
								cout << "ERROR: Unknown (imm11_0i >> 5) in ALUIMM SHR instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: Unknown funct3 in ALUIMM instruction " << IR << endl;
				}
				break;
			case ALURRR:  //ALU寄存器指令 
				switch(funct3) {
					case ADDSUB:
						switch(funct7) {
							case ADD:
								cout << "Do ADD" << endl;
								R[rd]=R[rs1]+R[rs2];
								break;
							case SUB:
								cout<<" Do SUB"<<endl;
								R[rd]=R[rs1]-R[rs2];
								break;
							default:
								cout << "ERROR: Unknown funct7 in ALURRR ADDSUB instruction " << IR << endl;
						}
						break;
					case SLL:
						cout<<"DO SLL"<<endl;
						unsigned int rsTransform;
						rsTransform=R[rs2]&0x1f;
						R[rd]=R[rs1]<<rsTransform; 
						break;
					case SLT:
						cout << "Do SLT " << endl;
						if((int)src1<(int)src2){
							R[rd]=1;
						}else{
							R[rd]=0;
						}
						break;
					case SLTU:
						cout << "Do SLTU" << endl;
						if(src2!=0){
							R[rd]=1;
						}else{
							R[rd]=0;
						}
						break;
					case XOR:
						cout << "Do XOR " << endl;
						R[rd]=R[rs1]^R[rs2];
						break;
					case OR:
						cout << "Do OR" << endl;
								R[rd]=R[rs1]|R[rs2];
							break;
					case AND:
						cout << "Do AND" << endl;
								R[rd]=R[rs1]&R[rs2];
						break;

					case SRLA:
						switch(funct7) {
							case SRL:
				           cout<<"DO SRL"<<endl;
                                           R[rd]=R[rs1]>>R[rs2];
								break;
							case SRA:
								  cout<<"DO SRA"<<endl;
								  R[rd]=(int)src1>>src2;
								break;
							default:
								cout << "ERROR: Unknown funct7 in ALURRR SRLA instruction " << IR << endl;
						}
						break;
					default:
						cout << "ERROR: Unknown funct3 in ALURRR instruction " << IR << endl;
				}
				break;
			case FENCES:
				switch(funct3) {
					case FENCE:
						//TODO: Fill code for the instruction here
						break;
					case FENCE_I:
						//TODO: Fill code for the instruction here
						cout<<"fence_i,nop"<<endl;
						break;
					default:
						cout << "ERROR: Unknown funct3 in FENCES instruction " << IR << endl;
				}
				break;
			case CSRX:
				switch(funct3) {
					case CALLBREAK:
						switch(Imm11_0ItypeZeroExtended) {
							case ECALL:
								//TODO: Fill code for the instruction here
								break;
						/*	case EBREAK:
								{//TODO: Fill code for the instruction here
									PC = ebreakadd;
									cout << "do ebreak and pc jumps to :" << ebreakadd << endl;
									break;
								}*/   //无法程序调用 
							default:
								cout << "ERROR: Unknown imm11_0i in CSRX CALLBREAK instruction " << IR << endl;
						}
						break;
					case CSRRW:
						//TODO: Fill code for the instruction here
						break;
					case CSRRS:
						//TODO: Fill code for the instruction here
						{
						    uint32_t temp = readWord(rs2)&0x00000fff;
							uint32_t temp1 = rs1 & 0x000fffff;
							writeWord(rd,(temp|temp1));
							cout << "do CSRRS and the result is :" << "rd="<<readWord(rd)<<endl;
							break;
						}
					case CSRRC:
						//TODO: Fill code for the instruction here
						break;
					case CSRRWI:
						//TODO: Fill code for the instruction here
						{	
						    if (rd == 0) break;
							else
							{
								uint32_t zmm = imm11j& 0x000001f;
								uint32_t tem = readWord(rs2) & 0x00000fff;
								writeWord(rd, tem);
								writeWord(rs2, zmm);
								cout << "do CSRRWI and the result is :" << "rd=" << readWord(rd) << endl;
								break;
							}
						}
					case CSRRSI:
						//TODO: Fill code for the instruction here
						break;
					case CSRRCI:
						//TODO: Fill code for the instruction here
						{	uint32_t zmm = imm11j & 0x000001f;
							uint32_t tem = readWord(rs2) & 0x00000fff;
							if (readWord(rd) != 0)
							{
								writeWord(rs2, zmm | tem);
							}
							cout << "do CSRRCI and the result is :" << "rd=" << readWord(rd) << endl;
							break;
						}
					default:
						cout << "ERROR: Unknown funct3 in CSRX instruction " << IR << endl;
				}
				break;
			default:
				cout << "ERROR: Unkown instruction " << IR << endl;
				break;
		}

		// Update PC
		PC = NextPC;

		cout << "Registers after executing the instruction" << endl;
		showRegs();

		cout << "Continue simulation (Y/n)? [Y]" << endl;
		cin.get(c);
		getchar();  //清除回车 
	}

	freeMem();

	return 0;
}


