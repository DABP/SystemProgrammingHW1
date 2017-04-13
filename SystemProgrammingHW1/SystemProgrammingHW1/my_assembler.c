/*
* ȭ�ϸ� : my_assembler.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
*
*/

/*
*
* ���α׷��� ����� �����Ѵ�.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* my_assembler ���α׷��� �������� �����͵��� ���ǵ� ���̴�.
*/

#include "my_assembler.h"

/* -----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	if (assem_pass2() < 0) {
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	//make_output("output") ; 
}
/* -----------------------------------------------------------------------------------
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*		   �����Ͽ���.
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
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
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
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
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ ������ ����.
*
*	===============================================================================
*		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
		tok = strtok_s(str_temp, "|", &context); // "|" ���ڸ� �������� tokizing�� �Ѵ�.
		inst[inst_index] = (struct inst_struct*)malloc(sizeof(struct inst_struct)); // inst �޸� �Ҵ�.
		inst[inst_index]->str = (char *)malloc(strlen(tok) + 1);
		strcpy_s(inst[inst_index]->str, strlen(tok) + 1, tok); // �� ó�� ��ū�� instruction �� �̸��� ����.

		while (tok = strtok_s(NULL, "|", &context)) {
			if (strcmp(tok, "\n") != 0) {
				if (cnt == 0) {
					inst[inst_index]->format = atoi(tok); // ����(format) ����
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
* ���� : ����� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� :
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
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �ҽ��ڵ��� ���ι�ȣ
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* -----------------------------------------------------------------------------------
*/

int tok_parsing(int index)
{
	/* add your code here */
	char *tok = NULL;
	char *context = NULL;
	char operand_tmp[40] = { 0, };

	tok = strtok_s(input_data[index], "\t", &context);

	// ù��°�� ���� ��� 
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
	else { // token_table[token_line]->label�� ������ 0���� �ʱ�ȭ
		initialize_label(token_line);
	}
	


	// operator_ �޸� �Ҵ� �� tok ����
	token_table[token_line]->operator_ = (char *)malloc(strlen(tok) + 1);
	strcpy_s(token_table[token_line]->operator_, strlen(tok) + 1, tok);

	if (strcmp(token_table[token_line]->operator_, "LTORG") == 0 || strcmp(token_table[token_line]->operator_, "CSECT") == 0) {
		initialize_operand(token_line, 0);
		initialize_comment(token_line);
		return 0;
	}

	if (strcmp(token_table[token_line]->operator_, "RSUB") != 0) { // RSUB �� �ƴ� ��
		if (strcmp(token_table[token_line]->operator_, "END") == 0) { // END �� ��
			tok = strtok_s(NULL, "\t", &context);
			token_table[token_line]->operand[0] = (char *)malloc(strlen(tok) + 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(tok) + 1, tok);
			
			initialize_operand(token_line, 1);
			initialize_comment(token_line);
			return 0;
		}

		if ((tok = strtok_s(NULL, "\t", &context)) != NULL) // operand
			strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand �ӽ� ����.
		else return 0;
	}
	else {
		operand_tmp[0] = '\0';
	}

	// comment �ʱ�ȭ �� ����
	if (!(tok = strtok_s(NULL, "\t", &context))) { // comment �� ���� ��,
		initialize_comment(token_line);
	}
	else {
		token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
		strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
	}

	// operand �ʱ�ȭ �� ����
	if (strlen(operand_tmp) > 0) {
		if (strchr(operand_tmp, ',') == NULL) { // operand�� ������ 1���� ��.
			token_table[token_line]->operand[0] = (char *)malloc(strlen(operand_tmp) + 1);
			initialize_operand(token_line, 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(operand_tmp) + 1, operand_tmp);

		}
		else { // operand �� ������ 2�� �̻�
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
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/

void make_objectcode(char *file_name)
{
	/* add your code here */

}

/* -----------------------------------------------------------------------------------
* ���� : token �� label�� 0���� �ʱ�ȭ �ϴ� �Լ��̴�.
* �Ű� : token �� index
* ��ȯ : ����
*
* -----------------------------------------------------------------------------------
*/
void initialize_label(int index) {
	token_table[index]->label = (char *)malloc(sizeof(char));
	token_table[index]->label[0] = '\0';
}

/* -----------------------------------------------------------------------------------
* ���� : token �� operand�� 0���� �ʱ�ȭ �ϴ� �Լ��̴�.
* �Ű� : token �� index, �ʱ�ȭ�� operand ���� start_num
* ��ȯ : ����
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
* ���� : token �� comment�� 0���� �ʱ�ȭ �ϴ� �Լ��̴�.
* �Ű� : token �� index
* ��ȯ : ����
*
* -----------------------------------------------------------------------------------
*/
void initialize_comment(int index) {
	token_table[index]->comment = (char *)malloc(sizeof(char));
	token_table[index]->comment[0] = '\0';
}


