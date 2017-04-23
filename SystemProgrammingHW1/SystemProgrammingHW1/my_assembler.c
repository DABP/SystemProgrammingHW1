/*
* 화일명 : my_assembler.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
*
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler.h"

/* ----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*		   또한 중간파일을 생성하지 않는다.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}
	// make_opcode_output("output");



	if (assem_pass1() < 0) {
		printf("assem_pass1: Fail.  \n");
		return -1;
	}
	if (assem_pass2() < 0) {
		printf(" assem_pass2: Fail.  \n");
		return -1;
	}

	make_objectcode_output("output");

	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*		   구현하였다.
* ----------------------------------------------------------------------------------
*/
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
*	===============================================================================
*
* ----------------------------------------------------------------------------------
*/
int init_inst_file(char *inst_file)
{
	FILE * file;

	fopen_s(&file, inst_file, "rb");
	if (file == NULL) {
		return -1;
	}

	inst_index = 0;
	char *tok;
	char *context;
	char str_temp[255];

	while (!feof(file)) {
		fgets(str_temp, sizeof(str_temp), file);
		tok = NULL;
		context = NULL;
		int cnt = 0;
		tok = strtok_s(str_temp, "|", &context); // "|" 문자를 기준으로 tokizing을 한다.
		inst_table[inst_index] = (struct inst_unit*)malloc(sizeof(struct inst_unit)); // inst 메모리 할당.
		inst_table[inst_index]->name = (char *)malloc(strlen(tok) + 1);
		strcpy_s(inst_table[inst_index]->name, strlen(tok) + 1, tok); // 맨 처음 토큰인 instruction 의 이름을 저장.

		while (tok = strtok_s(NULL, "|", &context)) {
			if (strcmp(tok, "\n") != 0) {
				if (cnt == 0) {
					inst_table[inst_index]->format = atoi(tok); // 형식(format) 저장
				}
				else if (cnt == 1) {
					inst_table[inst_index]->opcode = strtoul(tok, NULL, 16); // opcode 저장. 16진수 형태로 변환하여 저장.
				}
				else if (cnt == 2) {
					inst_table[inst_index]->operand_num = atoi(tok); // operand 개수 저장.
				}
			}
			cnt++;
		}
		inst_index++; // inst_table의 라인 수
	}
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 :
*
* ----------------------------------------------------------------------------------
*/
int init_input_file(char *input_file)
{
	FILE * file;
	int errno_ = 0;

	/* add your code here */
	fopen_s(&file, input_file, "r");
	if (file == NULL) {
		return -1;
	}

	line_num = 0;
	char *tok;
	char *context;
	char str_temp[255];
	char *del;

	while (!feof(file)) {
		fgets(str_temp, sizeof(str_temp), file); // 1 line str_temp 에 저장.
		input_data[line_num] = (char *)malloc(strlen(str_temp) + 1); // str_temp 크기만큼 동적할당.
		strcpy_s(input_data[line_num], strlen(str_temp) + 1, str_temp); // input_data[index]에 읽어온 1 line 저장.
		while (del = strchr(input_data[line_num], '\n')) { // 개행('\n')을 없애줌.
			*del = '\0';
		}
		line_num++; // input_data 의 라인 수.
	}
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 소스코드의 라인번호
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* ----------------------------------------------------------------------------------
*/
int token_parsing(int index)
{
	/* add your code here */
	char *tok = NULL;
	char *context = NULL;
	char operand_tmp[40] = { 0, };
	int no_operand = 0;		// operand 개수가 0개이면 1.
	int ltorg_flag = 0;		// 리터럴이 정의되었으면 1. (operator가 "END" or "CSECT"가 나왔는데 0이면 표시)

	tok = strtok_s(input_data[index], "\t", &context); // label or operator

	// 첫번째가 라벨인 경우 
	if (search_opcode(tok) < 0 &&
		strcmp(tok, "END") &&
		strcmp(tok, "EXTDEF") &&
		strcmp(tok, "EXTREF") &&
		strcmp(tok, "LTORG") &&
		strcmp(tok, "EQU") &&
		strcmp(tok, "CSECT")) {
		if (tok[0] == '+') {
			initialize_label(token_line);
		}
		else {
			token_table[token_line]->label = (char *)malloc(strlen(tok) + 1);
			strcpy_s(token_table[token_line]->label, strlen(tok) + 1, tok);

			tok = strtok_s(NULL, "\t", &context); // operator
		}
	}
	else { // token_table[token_line]->label을 사이즈 0으로 초기화
		initialize_label(token_line);
	}

	// operator_ 메모리 할당 및 tok 복사
	token_table[token_line]->operator_ = (char *)malloc(strlen(tok) + 1);
	strcpy_s(token_table[token_line]->operator_, strlen(tok) + 1, tok);

	if ((tok = strtok_s(NULL, "\t", &context)) != NULL) { // operand
		int op_idx = search_opcode(token_table[token_line]->operator_);

		// instruction이 아닌 경우 예외처리. (op_idx가 -1임)
		if (strcmp(token_table[token_line]->operator_, "START") == 0 ||
			strcmp(token_table[token_line]->operator_, "EXTDEF") == 0 ||
			strcmp(token_table[token_line]->operator_, "EXTREF") == 0 ||
			strcmp(token_table[token_line]->operator_, "RESB") == 0 ||
			strcmp(token_table[token_line]->operator_, "RESW") == 0 ||
			strcmp(token_table[token_line]->operator_, "BYTE") == 0 ||
			strcmp(token_table[token_line]->operator_, "WORD") == 0 ||
			strcmp(token_table[token_line]->operator_, "EQU") == 0 ||
			strcmp(token_table[token_line]->operator_, "END") == 0) {
			strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand 임시 저장.
		}
		else {
			if (op_idx > 0) {
				// operand 개수가 0개인 instruction 인 경우 comment 에 저장.
				if (inst_table[op_idx]->operand_num == 0) {
					initialize_operand(token_line, 0);
					token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
					strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
					no_operand = 1;
					//return 0;
				}
				strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand 임시 저장.
			}
		}
	}
	else {
		// operand와 comment가 존재하지 않는 경우 사이즈 0으로 초기화.
		initialize_operand(token_line, 0);
		initialize_comment(token_line);
	}

	// comment 초기화 및 대입
	if ((tok = strtok_s(NULL, "\t", &context)) != NULL) { // comment 가 있는 경우
		token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
		strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
	}
	else {
		// comment가 존재하지 않는 경우 사이즈 0으로 초기화.
		initialize_comment(token_line);
	}

	// operand 초기화 및 대입
	if (strlen(operand_tmp) > 0 && no_operand == 0) {
		if (strchr(operand_tmp, ',') == NULL) { // operand의 개수가 1개일 때.
			token_table[token_line]->operand[0] = (char *)malloc(strlen(operand_tmp) + 1);
			initialize_operand(token_line, 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(operand_tmp) + 1, operand_tmp);

			// instruction이 아닌 경우 예외 처리.
			int op_num = search_opcode(token_table[token_line]->operator_);
			if (strcmp(token_table[token_line]->operator_, "START") != 0 &&
				strcmp(token_table[token_line]->operator_, "EXTDEF") != 0 &&
				strcmp(token_table[token_line]->operator_, "EXTREF") != 0 &&
				strcmp(token_table[token_line]->operator_, "RESB") != 0 &&
				strcmp(token_table[token_line]->operator_, "RESW") != 0 &&
				strcmp(token_table[token_line]->operator_, "BYTE") != 0 &&
				strcmp(token_table[token_line]->operator_, "WORD") != 0 &&
				strcmp(token_table[token_line]->operator_, "EQU") != 0 &&
				strcmp(token_table[token_line]->operator_, "END") != 0) {
				if (inst_table[op_num]->operand_num != 1) {
					return -1;
				}
			}
		}
		else { // operand 의 개수가 2개 이상
			char *operand_tok = NULL;
			char *operand_context = NULL;
			operand_tok = strtok_s(operand_tmp, ",", &operand_context);
			token_table[token_line]->operand[0] = (char *)malloc(strlen(operand_tok) + 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(operand_tok) + 1, operand_tok);
			int cnt = 1;
			while (cnt < MAX_OPERAND) {
				if (operand_tok = strtok_s(NULL, ",", &operand_context)) {
					token_table[token_line]->operand[cnt] = (char *)malloc(strlen(operand_tok) + 1);
					strcpy_s(token_table[token_line]->operand[cnt], strlen(operand_tok) + 1, operand_tok);
				}
				else {
					initialize_operand(token_line, cnt);
					break;
				}
				cnt++;
			}
		}
	}
	else {
		// operand가 0개인 경우 사이즈 0으로 초기화.
		initialize_operand(token_line, 0);
	}
	
	if (token_table[token_line]->operand[0][0] == '=') {
		set_literal(token_table[token_line]->operand[0]);
	}


	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
*
* ----------------------------------------------------------------------------------
*/
int search_opcode(char *str)
{
	/* add your code here */
	int cnt = 0;
	char tmp[20];
	int cnt2 = 0;
	for (cnt = 0; cnt < inst_index; cnt++) { // inst_index 까지 검사.
		if (str[0] == '+') { // Extended인 경우 str[0] 삭제 후 한칸씩 당겨 tmp에 저장.
			for (cnt2 = 1; cnt2 <= strlen(str); cnt2++) {
				tmp[cnt2 - 1] = str[cnt2];
			}
			tmp[strlen(str) - 1] = '\0';
		}
		else {
			strcpy_s(tmp, strlen(str) + 1, str);
		}
		if (strcmp(tmp, inst_table[cnt]->name) == 0) {
			return cnt;
		}
	}
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */
	int cnt = 0;
	token_line = 0;
	for (cnt = 0; cnt < line_num; cnt++) {
		if (input_data[cnt][0] != '.') {
			token_table[token_line] = (struct token_unit*)malloc(sizeof(struct token_unit));
			token_parsing(cnt);
			token_line++;
		}
	}
	file_name = NULL;
	if (file_name == NULL) { // 파일 이름이 지정되어 있지 않은 경우 console에 출력
		cnt = 0;
		for (cnt = 0; cnt < token_line; cnt++) {
			printf("%s\t\t", token_table[cnt]->label);
			printf("%s\t", token_table[cnt]->operator_);
			printf("%s", token_table[cnt]->operand[0]);
			if (strlen(token_table[cnt]->operand[1]) > 0)
				printf(",");
			printf("%s", token_table[cnt]->operand[1]);
			if (strlen(token_table[cnt]->operand[2]) > 0)
				printf(",");
			printf("%s\t", token_table[cnt]->operand[2]);
			int idx = search_opcode(token_table[cnt]->operator_);
			if (idx > 0)
				printf("%02X", inst_table[idx]->opcode);
			printf("\n");
		}
	}
	else { // 파일 이름이 지정되어 있는 경우 해당 파일에 기록.
		fopen_s(&output_file, file_name, "w");
		cnt = 0;
		for (cnt = 0; cnt < token_line; cnt++) {
			fprintf(output_file, "%s\t\t", token_table[cnt]->label);
			fprintf(output_file, "%s\t", token_table[cnt]->operator_);
			fprintf(output_file, "%s", token_table[cnt]->operand[0]);
			if (strlen(token_table[cnt]->operand[1]) > 0)
				fprintf(output_file, ",");
			fprintf(output_file, "%s", token_table[cnt]->operand[1]);
			if (strlen(token_table[cnt]->operand[2]) > 0)
				fprintf(output_file, ",");
			fprintf(output_file, "%s\t", token_table[cnt]->operand[2]);
			int idx = search_opcode(token_table[cnt]->operator_);
			if (idx > 0)
				fprintf(output_file, "%02X", inst_table[idx]->opcode);
			fprintf(output_file, "\n");
		}
	}
}







/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	int cnt = 0;
	token_line = 0;
	literal_num = 0;
	sym_num = 0;
	for (cnt = 0; cnt < line_num; cnt++) {
		if (input_data[cnt][0] != '.') {
			token_table[token_line] = (struct token_unit*)malloc(sizeof(struct token_unit));
			token_parsing(cnt);
			set_location_counter();
			token_line++;
			if ((strcmp(token_table[token_line - 1]->operator_, "LTORG") == 0 || strcmp(token_table[token_line - 1]->operator_, "END") == 0) && literal_num > 0) {
				token_line += def_literal();
			}
			else if (strcmp(token_table[token_line - 1]->operator_, "START") == 0) {
				add_symbol(token_table[token_line - 1]->operand[0], token_table[token_line - 1]->label, token_table[token_line - 1]->label);
			}
			else if (strcmp(token_table[token_line - 1]->operator_, "EXTDEF") == 0) {
				int cnt2 = 0;
				for(cnt2 = 0; cnt2 < MAX_OPERAND; cnt2++)
				add_symbol(token_table[token_line - 1]->addr, token_table[token_line - 1]->operand[cnt2], token_table[token_line - 1]->)
			}
		}
	}

	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
	return 0;

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
	int cnt = 0;
	for (cnt = 0; cnt < token_line; cnt++) {
		if (strcmp(token_table[cnt]->operator_, "START") != 0 &&
			strcmp(token_table[cnt]->operator_, "EXTDEF") != 0 &&
			strcmp(token_table[cnt]->operator_, "EXTREF") != 0 &&
			strcmp(token_table[cnt]->operator_, "EQU") != 0 &&
			strcmp(token_table[cnt]->operator_, "LTORG") != 0 &&
			strcmp(token_table[cnt]->operator_, "END") != 0) {
			printf("%04X\t", token_table[cnt]->addr);
		}
		else
			printf("\t");
		printf("%s\t\t", token_table[cnt]->label);
		printf("%s\t", token_table[cnt]->operator_);
		printf("%s", token_table[cnt]->operand[0]);
		if (strlen(token_table[cnt]->operand[1]) > 0)
			printf(",");
		printf("%s", token_table[cnt]->operand[1]);
		if (strlen(token_table[cnt]->operand[2]) > 0)
			printf(",");
		printf("%s\t", token_table[cnt]->operand[2]);
		int idx = search_opcode(token_table[cnt]->operator_);
		if (idx > 0)
			printf("%02X", inst_table[idx]->opcode);
		printf("\n");
	}
}

/* --------------------------------------------------------------------------------*
* ------------------------------ 추가로 구현한 함수 -------------------------------*
* --------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------------
* 설명 : token 의 label을 0으로 초기화 하는 함수이다.
* 매계 : token 의 index
* 반환 : 없음
*
* -----------------------------------------------------------------------------------
*/
void initialize_label(int index) {
	token_table[index]->label = (char *)malloc(sizeof(char));
	token_table[index]->label[0] = '\0';
}

/* -----------------------------------------------------------------------------------
* 설명 : token 의 operand을 0으로 초기화 하는 함수이다.
* 매계 : token 의 index, 초기화할 operand 시작 start_num
* 반환 : 없음
*
* -----------------------------------------------------------------------------------
*/
void initialize_operand(int index, int start_num) {
	int cnt = 0;
	for (cnt = start_num; cnt < MAX_OPERAND; cnt++) {
		token_table[index]->operand[cnt] = (char *)malloc(sizeof(char));
		token_table[index]->operand[cnt][0] = '\0';
	}
}

/* -----------------------------------------------------------------------------------
* 설명 : token 의 comment을 0으로 초기화 하는 함수이다.
* 매계 : token 의 index
* 반환 : 없음
*
* -----------------------------------------------------------------------------------
*/
void initialize_comment(int index) {
	token_table[index]->comment = (char *)malloc(sizeof(char));
	token_table[index]->comment[0] = '\0';
}

/* -----------------------------------------------------------------------------------
* 설명 : location counter를 초기화 한다. assem_pass1에서 사용.
* 매계 : void
* 반환 : void
*
* -----------------------------------------------------------------------------------
*/
void set_location_counter() {
	// address 초기화
	token_table[token_line]->addr = 0;
	if (strcmp(token_table[token_line]->operator_, "CSECT") == 0)
		locctr = 0;
	if (strcmp(token_table[token_line]->operator_, "START") != 0 &&
		strcmp(token_table[token_line]->operator_, "EXTDEF") != 0 &&
		strcmp(token_table[token_line]->operator_, "EXTREF") != 0 &&
		strcmp(token_table[token_line]->operator_, "EQU") != 0 &&
		strcmp(token_table[token_line]->operator_, "END") != 0 &&
		strcmp(token_table[token_line]->operator_, "LTORG") != 0 &&
		strcmp(token_table[token_line]->operator_, "CSECT") != 0
		) {
		if (strcmp(token_table[token_line]->operator_, "RESB") == 0) {
			token_table[token_line]->addr = locctr;
			locctr += atoi(token_table[token_line]->operand[0]) * 1;
		}
		else if (strcmp(token_table[token_line]->operator_, "RESW") == 0) {
			token_table[token_line]->addr = locctr;
			locctr += atoi(token_table[token_line]->operand[0]) * 3;
		}
		else if (strcmp(token_table[token_line]->operator_, "BYTE") == 0) {
			token_table[token_line]->addr = locctr;
			locctr += 1;
		}
		else if (strcmp(token_table[token_line]->operator_, "WORD") == 0) {
			token_table[token_line]->addr = locctr;
			locctr += 3;
		}
		else {
			if (token_table[token_line]->operator_[0] == '+') { // 4형식 instruction
				token_table[token_line]->addr = locctr;
				locctr += 4;
			}
			else { // 1, 2, 3형식 instruction
				int format = inst_table[search_opcode(token_table[token_line]->operator_)]->format;
				token_table[token_line]->addr = locctr;
				locctr += format;
			}
		}
	}
}

/* -----------------------------------------------------------------------------------
* 설명 : tok_parsing중에 리터럴 상수가 나온 경우 literal_pool에 추가한다.
* 매계 : 리터럴 상수의 이름.
* 반환 : 없음.
*
* -----------------------------------------------------------------------------------
*/
void set_literal(char *str) {
	int cnt = 0;
	int is_duplicate = 0;
	for (cnt = 0; cnt < literal_num; cnt++) {
		if (strcmp(literal_table[cnt]->name, str) == 0) {
			is_duplicate = 1;
			return;
		}
	}
	if (is_duplicate == 0) {
		literal_table[literal_num] = (struct literal_pool*) malloc(sizeof(struct literal_pool));
		literal_table[literal_num]->name = (char *)malloc(strlen(str) + 1);
		strcpy_s(literal_table[literal_num]->name, strlen(str) + 1, str);
		literal_num++;
	}
}

/* -----------------------------------------------------------------------------------
* 설명 : literal_table 에 있는 리터럴들을 프로그램라인에 추가한다.
* 매계 : 없음.
* 반환 : 추가한 라인의 수.
*
* -----------------------------------------------------------------------------------
*/
int def_literal() {
	int cnt = 0;
	int cnt2 = 0;
	int return_num = 0;
	for (cnt = token_line, cnt2 = 0; cnt2 < literal_num; cnt++, cnt2++) {
		token_table[cnt] = (struct token_unit*)malloc(sizeof(struct token_unit));
		token_table[cnt]->addr = locctr;
		token_table[cnt]->label = (char *)malloc(sizeof('*') + sizeof('\0'));
		token_table[cnt]->label[0] = '*';
		token_table[cnt]->label[1] = '\0';
		token_table[cnt]->operator_ = (char *)malloc(strlen(literal_table[cnt2]->name) + 1);
		strcpy_s(token_table[cnt]->operator_, strlen(literal_table[cnt2]->name) + 1, literal_table[cnt2]->name);
		if (token_table[cnt]->operator_[1] == 'C') {
			locctr += (strlen(token_table[cnt]->operator_) - 4);
		}
		else if (token_table[cnt]->operator_[1] == 'X') {
			locctr += ((strlen(token_table[cnt]->operator_) - 4) / 2);
		}
		initialize_operand(cnt, 0);
		initialize_comment(cnt);
	}
	return_num = literal_num;
	literal_num = 0;			// 리터럴 개수 초기화.
	return return_num;
}

/* -----------------------------------------------------------------------------------
* 설명 : sym_table에 심볼 추가.
* 매계 : 심볼의 주소, 심볼 이름, 심볼이 속해있는 섹션 이름.
* 반환 : void
*
* -----------------------------------------------------------------------------------
*/
void add_symbol(int address, char *name, char *section) {
	sym_table[sym_num] = (struct symbol_unit*)malloc(sizeof(struct symbol_unit));
	sym_table[sym_num]->addr = address;
	strcpy_s(sym_table[sym_num]->symbol, strlen(name) + 1, name);
	strcpy_s(sym_table[sym_num]->section, strlen(section) + 1, section);
}

/* -----------------------------------------------------------------------------------
* 설명 :
* 매계 :
* 반환 :
*
* -----------------------------------------------------------------------------------
*/
