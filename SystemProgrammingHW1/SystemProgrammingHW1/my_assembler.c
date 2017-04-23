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
#include <fcntl.h>

#include "my_assembler.h"

/* ----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
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
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*		   �����Ͽ���.
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
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
*
*	===============================================================================
*		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
		tok = strtok_s(str_temp, "|", &context); // "|" ���ڸ� �������� tokizing�� �Ѵ�.
		inst_table[inst_index] = (struct inst_unit*)malloc(sizeof(struct inst_unit)); // inst �޸� �Ҵ�.
		inst_table[inst_index]->name = (char *)malloc(strlen(tok) + 1);
		strcpy_s(inst_table[inst_index]->name, strlen(tok) + 1, tok); // �� ó�� ��ū�� instruction �� �̸��� ����.

		while (tok = strtok_s(NULL, "|", &context)) {
			if (strcmp(tok, "\n") != 0) {
				if (cnt == 0) {
					inst_table[inst_index]->format = atoi(tok); // ����(format) ����
				}
				else if (cnt == 1) {
					inst_table[inst_index]->opcode = strtoul(tok, NULL, 16); // opcode ����. 16���� ���·� ��ȯ�Ͽ� ����.
				}
				else if (cnt == 2) {
					inst_table[inst_index]->operand_num = atoi(tok); // operand ���� ����.
				}
			}
			cnt++;
		}
		inst_index++; // inst_table�� ���� ��
	}
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� :
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
		fgets(str_temp, sizeof(str_temp), file); // 1 line str_temp �� ����.
		input_data[line_num] = (char *)malloc(strlen(str_temp) + 1); // str_temp ũ�⸸ŭ �����Ҵ�.
		strcpy_s(input_data[line_num], strlen(str_temp) + 1, str_temp); // input_data[index]�� �о�� 1 line ����.
		while (del = strchr(input_data[line_num], '\n')) { // ����('\n')�� ������.
			*del = '\0';
		}
		line_num++; // input_data �� ���� ��.
	}
	fclose(file);
	return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �ҽ��ڵ��� ���ι�ȣ
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* ----------------------------------------------------------------------------------
*/
int token_parsing(int index)
{
	/* add your code here */
	char *tok = NULL;
	char *context = NULL;
	char operand_tmp[40] = { 0, };
	int no_operand = 0;		// operand ������ 0���̸� 1.
	int ltorg_flag = 0;		// ���ͷ��� ���ǵǾ����� 1. (operator�� "END" or "CSECT"�� ���Դµ� 0�̸� ǥ��)

	tok = strtok_s(input_data[index], "\t", &context); // label or operator

	// ù��°�� ���� ��� 
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
	else { // token_table[token_line]->label�� ������ 0���� �ʱ�ȭ
		initialize_label(token_line);
	}

	// operator_ �޸� �Ҵ� �� tok ����
	token_table[token_line]->operator_ = (char *)malloc(strlen(tok) + 1);
	strcpy_s(token_table[token_line]->operator_, strlen(tok) + 1, tok);

	if ((tok = strtok_s(NULL, "\t", &context)) != NULL) { // operand
		int op_idx = search_opcode(token_table[token_line]->operator_);

		// instruction�� �ƴ� ��� ����ó��. (op_idx�� -1��)
		if (strcmp(token_table[token_line]->operator_, "START") == 0 ||
			strcmp(token_table[token_line]->operator_, "EXTDEF") == 0 ||
			strcmp(token_table[token_line]->operator_, "EXTREF") == 0 ||
			strcmp(token_table[token_line]->operator_, "RESB") == 0 ||
			strcmp(token_table[token_line]->operator_, "RESW") == 0 ||
			strcmp(token_table[token_line]->operator_, "BYTE") == 0 ||
			strcmp(token_table[token_line]->operator_, "WORD") == 0 ||
			strcmp(token_table[token_line]->operator_, "EQU") == 0 ||
			strcmp(token_table[token_line]->operator_, "END") == 0) {
			strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand �ӽ� ����.
		}
		else {
			if (op_idx > 0) {
				// operand ������ 0���� instruction �� ��� comment �� ����.
				if (inst_table[op_idx]->operand_num == 0) {
					initialize_operand(token_line, 0);
					token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
					strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
					no_operand = 1;
					//return 0;
				}
				strcpy_s(operand_tmp, strlen(tok) + 1, tok); // operand �ӽ� ����.
			}
		}
	}
	else {
		// operand�� comment�� �������� �ʴ� ��� ������ 0���� �ʱ�ȭ.
		initialize_operand(token_line, 0);
		initialize_comment(token_line);
	}

	// comment �ʱ�ȭ �� ����
	if ((tok = strtok_s(NULL, "\t", &context)) != NULL) { // comment �� �ִ� ���
		token_table[token_line]->comment = (char *)malloc(strlen(tok) + 1);
		strcpy_s(token_table[token_line]->comment, strlen(tok) + 1, tok);
	}
	else {
		// comment�� �������� �ʴ� ��� ������ 0���� �ʱ�ȭ.
		initialize_comment(token_line);
	}

	// operand �ʱ�ȭ �� ����
	if (strlen(operand_tmp) > 0 && no_operand == 0) {
		if (strchr(operand_tmp, ',') == NULL) { // operand�� ������ 1���� ��.
			token_table[token_line]->operand[0] = (char *)malloc(strlen(operand_tmp) + 1);
			initialize_operand(token_line, 1);
			strcpy_s(token_table[token_line]->operand[0], strlen(operand_tmp) + 1, operand_tmp);

			// instruction�� �ƴ� ��� ���� ó��.
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
		// operand�� 0���� ��� ������ 0���� �ʱ�ȭ.
		initialize_operand(token_line, 0);
	}
	
	if (token_table[token_line]->operand[0][0] == '=') {
		set_literal(token_table[token_line]->operand[0]);
	}


	return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
*
* ----------------------------------------------------------------------------------
*/
int search_opcode(char *str)
{
	/* add your code here */
	int cnt = 0;
	char tmp[20];
	int cnt2 = 0;
	for (cnt = 0; cnt < inst_index; cnt++) { // inst_index ���� �˻�.
		if (str[0] == '+') { // Extended�� ��� str[0] ���� �� ��ĭ�� ��� tmp�� ����.
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
	if (file_name == NULL) { // ���� �̸��� �����Ǿ� ���� ���� ��� console�� ���
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
	else { // ���� �̸��� �����Ǿ� �ִ� ��� �ش� ���Ͽ� ���.
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
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
	return 0;

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ------------------------------ �߰��� ������ �Լ� -------------------------------*
* --------------------------------------------------------------------------------*/

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

/* -----------------------------------------------------------------------------------
* ���� : location counter�� �ʱ�ȭ �Ѵ�. assem_pass1���� ���.
* �Ű� : void
* ��ȯ : void
*
* -----------------------------------------------------------------------------------
*/
void set_location_counter() {
	// address �ʱ�ȭ
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
			if (token_table[token_line]->operator_[0] == '+') { // 4���� instruction
				token_table[token_line]->addr = locctr;
				locctr += 4;
			}
			else { // 1, 2, 3���� instruction
				int format = inst_table[search_opcode(token_table[token_line]->operator_)]->format;
				token_table[token_line]->addr = locctr;
				locctr += format;
			}
		}
	}
}

/* -----------------------------------------------------------------------------------
* ���� : tok_parsing�߿� ���ͷ� ����� ���� ��� literal_pool�� �߰��Ѵ�.
* �Ű� : ���ͷ� ����� �̸�.
* ��ȯ : ����.
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
* ���� : literal_table �� �ִ� ���ͷ����� ���α׷����ο� �߰��Ѵ�.
* �Ű� : ����.
* ��ȯ : �߰��� ������ ��.
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
	literal_num = 0;			// ���ͷ� ���� �ʱ�ȭ.
	return return_num;
}

/* -----------------------------------------------------------------------------------
* ���� : sym_table�� �ɺ� �߰�.
* �Ű� : �ɺ��� �ּ�, �ɺ� �̸�, �ɺ��� �����ִ� ���� �̸�.
* ��ȯ : void
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
* ���� :
* �Ű� :
* ��ȯ :
*
* -----------------------------------------------------------------------------------
*/
