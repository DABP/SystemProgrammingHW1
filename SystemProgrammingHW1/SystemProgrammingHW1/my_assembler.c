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

/*
* my_assembler 프로그램의 의존적인 데이터들이 정의된 곳이다.
*/

#include "my_assembler.h"

/* -----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*		   또한 중간파일을 생성하지 않는다.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	if (assem_pass2() < 0) {
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	//make_output("output") ; 
}
/* -----------------------------------------------------------------------------------
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*		   구현하였다.
* -----------------------------------------------------------------------------------
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

/* -----------------------------------------------------------------------------------
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
	for (cnt = 0; cnt < line_num; cnt++) {
		if (input_data[cnt][0] != '.') {
			token_table[token_line] = (struct token_unit*)malloc(sizeof(struct token_unit));
			tok_parsing(cnt);
			token_line++;
		}
	}

	return 0;
}

/* -----------------------------------------------------------------------------------
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
	int cnt = 0;
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
			printf("%02X", inst[idx]->op);
		printf("\n");
	}
	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
*	===============================================================================
*
* -----------------------------------------------------------------------------------
*/

int init_inst_file(char *inst_file)
{
	/* add your code here */
	FILE *inst_file_pointer;
	fopen_s(&inst_file_pointer, inst_file, "rb");
	if (inst_file_pointer == NULL) {
		return -1;
	}

	inst_index = 0;
	char *tok;
	char *context;
	char str_temp[255];

	while (!feof(inst_file_pointer)) {

		fgets(str_temp, sizeof(str_temp), inst_file_pointer);
		tok = NULL;
		context = NULL;

		int cnt = 0;
		tok = strtok_s(str_temp, "|", &context); // "|" 문자를 기준으로 tokizing을 한다.
		inst[inst_index] = (struct inst_struct*)malloc(sizeof(struct inst_struct)); // inst 메모리 할당.
		inst[inst_index]->str = (char *)malloc(strlen(tok) + 1);
		strcpy_s(inst[inst_index]->str, strlen(tok) + 1, tok); // 맨 처음 토큰인 instruction 의 이름을 저장.

		while (tok = strtok_s(NULL, "|", &context)) {
			if (strcmp(tok, "\n") != 0) {
				if (cnt == 0) {
					inst[inst_index]->format = atoi(tok); // 형식(format) 저장
				}
				else if (cnt == 1) {
					inst[inst_index]->op = strtoul(tok, NULL, 16);
				}
				else if (cnt == 2) {
					inst[inst_index]->ops = atoi(tok);
				}
			}
			cnt++;
		}
		inst_index++;
	}
	fclose(inst_file_pointer);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/


int init_input_file(char *input_file)
{
	/* add your code here */
	FILE *input_file_pointer;
	fopen_s(&input_file_pointer, input_file, "r");
	if (input_file_pointer == NULL) {
		return -1;
	}

	line_num = 0;
	char *tok;
	char *context;
	char str_temp[255];
	char *del;

	while (!feof(input_file_pointer)) {
		fgets(str_temp, sizeof(str_temp), input_file_pointer);
		input_data[line_num] = (char *)malloc(strlen(str_temp) + 1);
		strcpy_s(input_data[line_num], strlen(str_temp) + 1, str_temp);
		while (del = strchr(input_data[line_num], '\n')) {
			*del = '\0';
		}
		line_num++;
	}
	fclose(input_file_pointer);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 소스코드의 라인번호
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* -----------------------------------------------------------------------------------
*/

int tok_parsing(int index)
{
	/* add your code here */
	char *tok = NULL;
	char *context = NULL;
	char operand_tmp[40] = { 0, };

	tok = strtok_s(input_data[index], "\t", &context);

	// 첫번째가 라벨인 경우 
	if (search_opcode(tok) < 0 && strcmp(tok, "END") && strcmp(tok, "EXTDEF") && strcmp(tok, "EXTREF") && strcmp(tok, "LTORG") && strcmp(tok, "EQU") && strcmp(tok, "CSECT")) {
		if (tok[0] != '+') {
			token_table[token_line]->label = (char *)malloc(strlen(tok) + 1);
			strcpy_s(token_table[token_line]->label, strlen(tok) + 1, tok);

			tok = strtok_s(NULL, "\t", &context); // operator
		}
		else {
			initialize_label(token_line);
		}
		
	}
	else { // token_table[token_line]->label을 사이즈 0으로 초기화
		initialize_label(token_line);
	}
	


	// operator_ 메모리 할당 및 tok 복사
	token_table[token_line]->operator_ = (char *)malloc(strlen(tok) + 1);
	strcpy_s(token_table[token_line]->operator_, strlen(tok) + 1, tok);

	if (strcmp(token_table[token_line]->operator_, "LTORG") == 0 || strcmp(token_table[token_line]->operator_, "CSECT") == 0) {
		initialize_operand(token_line, 0);
		initialize_comment(token_line);
		return 0;
	}

	if (strcmp(token_table[token_line]->operator_, "RSUB") != 0) { // RSUB 이 아닐 때
		if (strcmp(token_table[token_line]->operator_, "END") == 0) { // END 일 때
			tok = strtok_s(NULL, "\t", &context);
			token_table[token_line]->operand[0] = (char *)malloc(strlen(tok) + 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(tok) + 1, tok);
			
			initialize_operand(token_line, 1);
			initialize_comment(token_line);
			return 0;
		}

		if ((tok = strtok_s(NULL, "\t", &context)) != NULL) // operand
			strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand 임시 저장.
		else return 0;
	}
	else {
		operand_tmp[0] = '\0';
	}

	// comment 초기화 및 대입
	if (!(tok = strtok_s(NULL, "\t", &context))) { // comment 가 없을 때,
		initialize_comment(token_line);
	}
	else {
		token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
		strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
	}

	// operand 초기화 및 대입
	if (strlen(operand_tmp) > 0) {
		if (strchr(operand_tmp, ',') == NULL) { // operand의 개수가 1개일 때.
			token_table[token_line]->operand[0] = (char *)malloc(strlen(operand_tmp) + 1);
			initialize_operand(token_line, 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(operand_tmp) + 1, operand_tmp);

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
		initialize_operand(token_line, 0);
	}
	/*
	int cnt = 0;
	for (cnt = 0; cnt < MAX_OPERAND; cnt++) {
		if (token_table[token_line]->operand[cnt][strlen(token_table[token_line]->operand[cnt]) - 1] == '\n')
			token_table[token_line]->operand[cnt][strlen(token_table[token_line]->operand[cnt]) - 1] = '\0';
	}
	if (token_table[token_line]->comment[strlen(token_table[token_line]->comment) - 1] == '\n')
		token_table[token_line]->comment[strlen(token_table[token_line]->comment) - 1] = '\0';
*/
	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_opcode(char *str)
{
	/* add your code here */
	int cnt = 0;
	for (cnt = 0; cnt < inst_index; cnt++) {
		if (strcmp(str, inst[cnt]->str) == 0) {
			return cnt;
		}
	}
	return -1;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/

void make_objectcode(char *file_name)
{
	/* add your code here */

}

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


