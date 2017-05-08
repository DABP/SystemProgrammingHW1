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
			if (op_idx >= 0) {
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
			if (idx >= 0)
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
			if (idx >= 0)
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
				strcpy_s(now_section, strlen(token_table[token_line - 1]->label) + 1, token_table[token_line - 1]->label);
			}
			else if (strcmp(token_table[token_line - 1]->operator_, "EXTDEF") == 0) {
				int cnt2 = 0;
				for (cnt2 = 0; (cnt2 < MAX_OPERAND) && (strlen(token_table[token_line - 1]->operand[cnt2]) > 0); cnt2++)
					add_symbol(0, token_table[token_line - 1]->operand[cnt2], now_section);
			}
			else if (strcmp(token_table[token_line - 1]->operator_, "EXTREF") == 0) {
				int cnt2 = 0;
				for (cnt2 = 0; (cnt2 < MAX_OPERAND) && (strlen(token_table[token_line - 1]->operand[cnt2]) > 0); cnt2++)
					add_symbol(0, token_table[token_line - 1]->operand[cnt2], now_section);
			}
			else if (strcmp(token_table[token_line - 1]->operator_, "CSECT") == 0) {
				strcpy_s(now_section, strlen(token_table[token_line - 1]->label) + 1, token_table[token_line - 1]->label);
			}
			else {
				// symbol �߰�.
				if (strlen(token_table[token_line - 1]->label) > 0) {
					add_symbol(token_table[token_line - 1]->addr, token_table[token_line - 1]->label, now_section);
				}
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
	int cnt = 0;
	for (cnt = 0; cnt < token_line; cnt++) {
		int op_idx = 0;
		int obcode = 0;
		int format = 0;
		token_table[cnt]->obcode = NULL;
		if (strcmp(token_table[cnt]->operator_, "START") == 0 || strcmp(token_table[cnt]->operator_, "CSECT") == 0) {
			strcpy_s(now_section, strlen(token_table[cnt]->label) + 1, token_table[cnt]->label);
		}
		else if (token_table[cnt]->operator_[0] == '=') { // literal
			if (token_table[cnt]->operator_[1] == 'C') {
				if (('A' <= token_table[cnt]->operator_[3] && token_table[cnt]->operator_[3] <= 'Z') ||
					('a' <= token_table[cnt]->operator_[3] && token_table[cnt]->operator_[3] <= 'z')) {
					obcode = token_table[cnt]->operator_[3] << 16;
					if (('A' <= token_table[cnt]->operator_[4] && token_table[cnt]->operator_[4] <= 'Z') ||
						('a' <= token_table[cnt]->operator_[4] && token_table[cnt]->operator_[4] <= 'z')) {
						obcode += token_table[cnt]->operator_[4] << 8;
						if (('A' <= token_table[cnt]->operator_[5] && token_table[cnt]->operator_[5] <= 'Z') ||
							('a' <= token_table[cnt]->operator_[5] && token_table[cnt]->operator_[5] <= 'z')) {
							obcode += token_table[cnt]->operator_[5];
						}
					}
				}
				char tmp[7];
				sprintf_s(tmp, 7, "%X", obcode);
				token_table[cnt]->obcode = (char *)malloc(strlen(tmp) + 1);
				strcpy_s(token_table[cnt]->obcode, strlen(tmp) + 1, tmp);
			}
			else if (token_table[cnt]->operator_[1] == 'X') {
				char tmp[10];
				strcpy_s(tmp, strlen(token_table[cnt]->operator_) + 1, token_table[cnt]->operator_);
				tmp[strlen(tmp) - 1] = '\0';

				token_table[cnt]->obcode = (char *)malloc(strlen(&tmp[3]) + 1);
				strcpy_s(token_table[cnt]->obcode, strlen(&tmp[3]) + 1, &tmp[3]);
			}

		}
		else if (strcmp(token_table[cnt]->operator_, "BYTE") == 0 || strcmp(token_table[cnt]->operator_, "WORD") == 0) {
			int len = 0;
			if (strcmp(token_table[cnt]->operator_, "BYTE") == 0)
				len = 2;
			else if (strcmp(token_table[cnt]->operator_, "WORD") == 0)
				len = 6;

			if (strchr(token_table[cnt]->operand[0], '\'') != NULL) {
				char tmp[10];
				strcpy_s(tmp, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				tmp[strlen(tmp) - 1] = '\0';

				token_table[cnt]->obcode = (char *)malloc(len + 1);
				strcpy_s(token_table[cnt]->obcode, len + 1, &tmp[2]);
			}
			else if (strchr(token_table[cnt]->operand[0], '+') != NULL) {
				char cpy[20];
				strcpy_s(cpy, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				char *deli = strchr(cpy, '+');
				char sym1[10];
				char sym2[10];
				*deli = '\0';

				strcpy_s(sym2, strlen(deli + 1) + 1, deli + 1);
				strcpy_s(sym1, strlen(cpy) + 1, cpy);

				int sym1_idx = search_symbol(sym1, now_section);
				int sym2_idx = search_symbol(sym2, now_section);

				int tmp = 0;
				tmp = (sym_table[sym1_idx]->addr) + (sym_table[sym2_idx]->addr);
				if (tmp < 0) { // 0���� �۾����� ���.
					tmp = 0x0FFF & tmp;
				}
				token_table[cnt]->obcode = (char *)malloc(len + 1);
				sprintf_s(token_table[cnt]->obcode, len + 1, "%0*X", len, tmp);
			}
			else if (strchr(token_table[cnt]->operand[0], '-') != NULL) {
				char cpy[20];
				strcpy_s(cpy, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				char *deli = strchr(cpy, '-');
				char sym1[10];
				char sym2[10];
				*deli = '\0';

				strcpy_s(sym2, strlen(deli + 1) + 1, deli + 1);
				strcpy_s(sym1, strlen(cpy) + 1, cpy);

				int sym1_idx = search_symbol(sym1, now_section);
				int sym2_idx = search_symbol(sym2, now_section);

				int tmp = 0;
				tmp = (sym_table[sym1_idx]->addr) - (sym_table[sym2_idx]->addr);
				if (tmp < 0) { // 0���� �۾����� ���.
					tmp = 0x0FFF & tmp;
				}
				token_table[cnt]->obcode = (char *)malloc(len + 1);
				sprintf_s(token_table[cnt]->obcode, len + 1, "%0*X", len, tmp);
			}
			else if (strchr(token_table[cnt]->operand[0], '*') != NULL) {
				char cpy[20];
				strcpy_s(cpy, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				char *deli = strchr(cpy, '*');
				char sym1[10];
				char sym2[10];
				*deli = '\0';

				strcpy_s(sym2, strlen(deli + 1) + 1, deli + 1);
				strcpy_s(sym1, strlen(cpy) + 1, cpy);

				int sym1_idx = search_symbol(sym1, now_section);
				int sym2_idx = search_symbol(sym2, now_section);

				int tmp = 0;
				tmp = (sym_table[sym1_idx]->addr) * (sym_table[sym2_idx]->addr);
				if (tmp < 0) { // 0���� �۾����� ���.
					tmp = 0x0FFF & tmp;
				}
				token_table[cnt]->obcode = (char *)malloc(len + 1);
				sprintf_s(token_table[cnt]->obcode, len + 1, "%0*X", len, tmp);
			}
			else if (strchr(token_table[cnt]->operand[0], '/') != NULL) {
				char cpy[20];
				strcpy_s(cpy, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				char *deli = strchr(cpy, '/');
				char sym1[10];
				char sym2[10];
				*deli = '\0';

				strcpy_s(sym2, strlen(deli + 1) + 1, deli + 1);
				strcpy_s(sym1, strlen(cpy) + 1, cpy);

				int sym1_idx = search_symbol(sym1, now_section);
				int sym2_idx = search_symbol(sym2, now_section);

				int tmp = 0;
				tmp = (sym_table[sym1_idx]->addr) / (sym_table[sym2_idx]->addr);
				if (tmp < 0) { // 0���� �۾����� ���.
					tmp = 0x0FFF & tmp;
				}
				token_table[cnt]->obcode = (char *)malloc(len + 1);
				sprintf_s(token_table[cnt]->obcode, len + 1, "%0*X", len, tmp);
			}
			else if (strchr(token_table[cnt]->operand[0], '%') != NULL) {
				char cpy[20];
				strcpy_s(cpy, strlen(token_table[cnt]->operand[0]) + 1, token_table[cnt]->operand[0]);
				char *deli = strchr(cpy, '%');
				char sym1[10];
				char sym2[10];
				*deli = '\0';

				strcpy_s(sym2, strlen(deli + 1) + 1, deli + 1);
				strcpy_s(sym1, strlen(cpy) + 1, cpy);

				int sym1_idx = search_symbol(sym1, now_section);
				int sym2_idx = search_symbol(sym2, now_section);

				int tmp = 0;
				tmp = (sym_table[sym1_idx]->addr) % (sym_table[sym2_idx]->addr);
				if (tmp < 0) { // 0���� �۾����� ���.
					tmp = 0x0FFF & tmp;
				}
				token_table[cnt]->obcode = (char *)malloc(len + 1);
				sprintf_s(token_table[cnt]->obcode, len + 1, "%0*X", len, tmp);
			}
			else {

			}
		}
		op_idx = search_opcode(token_table[cnt]->operator_);
		if (op_idx >= 0) {
			format = inst_table[op_idx]->format;
			if (format == 3) {
				if (token_table[cnt]->operator_[0] == '+')
					format = 4;
			}
			obcode = (int)(inst_table[op_idx]->opcode) << (format * 8 - 8);

			if (format == 3 || format == 4) {
				if (token_table[cnt]->operand[0][0] == '#') { // immediate
					obcode += 1 << format * 8 - 8;
					obcode += atoi(&(token_table[cnt]->operand[0][1]));
				}
				else {
					if (token_table[cnt]->operand[0][0] == '@') {// indirect
						obcode += 2 << format * 8 - 8;
					}
					else { // simple addressing
						obcode += 3 << format * 8 - 8;
					}
					int cnt2 = 0;
					// x
					for (cnt2 = 0; cnt2 < MAX_OPERAND; cnt2++) {
						if (strcmp(token_table[cnt]->operand[cnt2], "X") == 0) { // use X register
							obcode += 1 << format * 8 - 9;
						}
					}
					if (format == 3) {
						// b
						if (strcmp(token_table[cnt]->operator_, "LDB") == 0 ||
							strcmp(token_table[cnt]->operator_, "STB") == 0) {
							obcode += 1 << format * 8 - 10;
						}
						// p
						else if (strlen(token_table[cnt]->operand[0]) > 0) {
							obcode += 1 << format * 8 - 11;
						}
					}
					// e
					else {
						obcode += 1 << format * 8 - 12;
					}
					// display
					if (format == 3) {

						int sym_idx = search_symbol(token_table[cnt]->operand[0], now_section);
						if (sym_idx >= 0) {
							int tmp = 0;
							tmp = (sym_table[sym_idx]->addr) - (token_table[cnt + 1]->addr);
							if (tmp < 0) { // 0���� �۾����� ���.
								tmp = 0x0FFF & tmp;
							}
							obcode += tmp;
						}
						else if (token_table[cnt]->operand[0][0] == '=') { // literal
							int cnt2 = 0;
							int tmp = 0;
							for (cnt2 = 0; cnt2 < token_line; cnt2++) {
								if (strcmp(token_table[cnt]->operand[0], token_table[cnt2]->operator_) == 0) {
									tmp = (token_table[cnt2]->addr) - (token_table[cnt + 1]->addr);
									if (tmp < 0) {
										tmp = 0x0FFF & tmp;
									}
									obcode += tmp;
									break;
								}
							}
						}
					}
				}
			}
			else if (format == 2) {
				int cnt2 = 0;
				for (cnt2 = 0; cnt2 < inst_table[op_idx]->operand_num; cnt2++) {
					if (strlen(token_table[cnt]->operand[cnt2]) > 0) {
						int register_num = 0;
						if (cnt2 == 0) {
							register_num = get_register_num(token_table[cnt]->operand[cnt2]);
							if (register_num >= 0) {
								obcode += register_num << 4;
							}
						}
						else if (cnt2 == 1) {
							register_num = get_register_num(token_table[cnt]->operand[cnt2]);
							if (register_num >= 0) {
								obcode += register_num;
							}
						}
					}
				}
			}

			token_table[cnt]->obcode = (char *)malloc(sizeof(char) * 2 * format + 1);
			sprintf_s(token_table[cnt]->obcode, sizeof(char) * 2 * format + 1, "%0*X", format * 2, obcode);
		}
	}
	for (cnt = 0; cnt < token_line; cnt++) {
		if (strcmp(token_table[cnt]->operator_, "EXTDEF") != 0 &&
			strcmp(token_table[cnt]->operator_, "EXTREF") != 0 &&
			strcmp(token_table[cnt]->operator_, "LTORG") != 0 &&
			strcmp(token_table[cnt]->operator_, "CSECT") != 0 &&
			strcmp(token_table[cnt]->operator_, "END") != 0) {
			printf("%04X\t", token_table[cnt]->addr);
		}
		else {
			if (strcmp(token_table[cnt]->operator_, "CSECT") == 0)
				printf("\n");

			printf("\t");
		}
		printf("%s\t\t", token_table[cnt]->label);
		printf("%s\t", token_table[cnt]->operator_);
		printf("%s", token_table[cnt]->operand[0]);
		if (strlen(token_table[cnt]->operand[1]) > 0)
			printf(",");
		printf("%s", token_table[cnt]->operand[1]);
		if (strlen(token_table[cnt]->operand[2]) > 0)
			printf(",");
		printf("%s\t", token_table[cnt]->operand[2]);
		if (token_table[cnt]->obcode != NULL)
			printf("%s", token_table[cnt]->obcode);
		printf("\n");
	}
	printf("\n\n");
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
	ob_line_num = 0;
	int full_len = 0;
	for (cnt = 0; cnt < token_line; cnt++) {
		char _obcode[70] = { 0, };

		if (strcmp(token_table[cnt]->operator_, "START") == 0 ||
			strcmp(token_table[cnt]->operator_, "EXTDEF") == 0 ||
			strcmp(token_table[cnt]->operator_, "EXTREF") == 0 ||
			strcmp(token_table[cnt]->operator_, "END") == 0 ||
			strcmp(token_table[cnt]->operator_, "CSECT") == 0
			) {
			if (cnt == 0) {
				if (strcmp(token_table[cnt]->operator_, "START") == 0) {
					_obcode[0] = 'H';

					strcpy_s(now_section, strlen(token_table[cnt]->label) + 1, token_table[cnt]->label);	// ���� �ʱ�ȭ.
					_obcode[0] = 'H'; // 1��°�� H���ڵ� ǥ��
					strcpy_s(&_obcode[1], strlen(token_table[cnt]->label) + 1, token_table[cnt]->label); // ���α׷� �̸� ����
					int cnt2 = 0;
					for (cnt2 = strlen(token_table[cnt]->label) + 1; cnt2 < 7; cnt2++) { // �����ڸ� ����
						_obcode[cnt2] = ' ';
					}
					_obcode[19] = '\0';
					sprintf_s(&_obcode[7], 7, "%06X", atoi(token_table[cnt]->operand[0]));
					sprintf_s(&_obcode[13], 7, "000000");
				}
				else return -1;
			}
			else {
				if (strcmp(token_table[cnt]->operator_, "EXTDEF") == 0) {
					_obcode[0] = 'D';
					int cnt2 = 0;
					for (cnt2 = 0; cnt2 < MAX_OPERAND; cnt2++) {
						if (strlen(token_table[cnt]->operand[cnt2]) > 0) {
							strcpy_s(&_obcode[1 + cnt2 * 12], strlen(token_table[cnt]->operand[cnt2]) + 1, token_table[cnt]->operand[cnt2]);
							int sym = 0;
							sym = search_symbol(token_table[cnt]->operand[cnt2], now_section);
							if (sym < 0)
								return -1;
							sprintf_s(&_obcode[1 + cnt2 * 12 + 6], 7, "%06X", sym_table[sym]->addr);

						}
					}
				}
				else if (strcmp(token_table[cnt]->operator_, "EXTREF") == 0) {
					_obcode[0] = 'R';
					int cnt2 = 0;
					for (cnt2 = 0; cnt2 < MAX_OPERAND; cnt2++) {
						if (strlen(token_table[cnt]->operand[cnt2]) > 0) {
							strcpy_s(&_obcode[1 + cnt2 * 6], strlen(token_table[cnt]->operand[cnt2]) + 1, token_table[cnt]->operand[cnt2]);
							int cnt3 = 0;
							for (cnt3 = cnt2 * 6; !('A' <= _obcode[cnt3] && _obcode[cnt3] <= 'Z'); cnt3--) {
								_obcode[cnt3] = ' ';
							}
						}
					}
				}
				else if (strcmp(token_table[cnt]->operator_, "END") == 0) {
					if (token_table[cnt + 1] != NULL) {
						//literal
						int cnt2 = 0;
						for (cnt2 = cnt + 1; token_table[cnt2] != NULL; cnt2++) {
							if (strlen(object_codes[ob_line_num - 1]) + strlen(token_table[cnt + 1]->obcode) < sizeof(char) * 70) {
								char tmp[70];
								strcpy_s(tmp, strlen(object_codes[ob_line_num - 1]) + 1, object_codes[ob_line_num - 1]);
								free(object_codes[ob_line_num - 1]);
								char len_str[3];
								len_str[0] = tmp[7];
								len_str[1] = tmp[8];
								len_str[2] = '\0';
								char *stop;
								int len = strtol(len_str, &stop, 16);
								
								len += strlen(token_table[cnt + 1]->obcode) / 2;
								full_len += strlen(token_table[cnt + 1]->obcode);
								sprintf_s(len_str, sizeof(char) * 3, "%02X", len);
								tmp[7] = len_str[0];
								tmp[8] = len_str[1];
								object_codes[ob_line_num - 1] = (char *)malloc(strlen(tmp) + 1 + strlen(token_table[cnt + 1]->obcode));
								strcat_s(tmp, strlen(tmp) + strlen(token_table[cnt + 1]->obcode) + 1, token_table[cnt + 1]->obcode);
								strcpy_s(object_codes[ob_line_num - 1], strlen(tmp) + 1, tmp);
							}
							else {
								int t_len = 0;
								char m_record[17];
								int end_idx = 0;

								_obcode[0] = 'T';
							
								sprintf_s(&_obcode[1], sizeof(char) * 7, "%06X", full_len / 2);
								_obcode[7] = _obcode[8] = '0';
								_obcode[9] = '\0';
								int cnt3 = 0;
								for (cnt3 = cnt2; ; cnt3++, cnt2++) {
									if (token_table[cnt3] == NULL) {
										break;
									}
									else {
										if (token_table[cnt3]->obcode != NULL) {
											if (strlen(token_table[cnt3]->obcode) + t_len <= 60) {
												if (strlen(_obcode) == 9) {
													char addr[5];
													sprintf_s(addr, sizeof(char) * 5, "%04X", token_table[cnt3]->addr);
													_obcode[3] = addr[0];
													_obcode[4] = addr[1];
													_obcode[5] = addr[2];
													_obcode[6] = addr[3];
												}
												strcpy_s(&_obcode[strlen(_obcode)], strlen(token_table[cnt3]->obcode) + 1, token_table[cnt3]->obcode);
												t_len += strlen(token_table[cnt3]->obcode);
												full_len += strlen(token_table[cnt3]->obcode) / 2;
											}
											else {
												break;
											}
										}
										else {
											break;
										}
									}
								}
								char tmp[3];
								sprintf_s(tmp, sizeof(char) * 3, "%02X", t_len / 2);
								_obcode[7] = tmp[0];
								_obcode[8] = tmp[1];
								object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
								strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
								ob_line_num++;
								cnt--;
							}
						}
					}
					_obcode[0] = 'E';
					int cnt2;
					for (cnt2 = ob_line_num - 1; cnt2 >= 0; cnt2--) {
						if (object_codes[cnt2][0] == 'H') {
							sprintf_s(&object_codes[cnt2][13], sizeof(char) * 7, "%06X", full_len / 2);
							break;
						}
					}
					sprintf_s(&_obcode[1], sizeof(char) * 7, "%06X", atoi(token_table[0]->operand[0]));
					full_len = 0;

					object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
					strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
					ob_line_num++;

					break;
				}
				else if (strcmp(token_table[cnt]->operator_, "CSECT") == 0) {
					int cnt2;
					for (cnt2 = ob_line_num - 1; cnt2 >= 0; cnt2--) {
						if (object_codes[cnt2][0] == 'H') {
							sprintf_s(&object_codes[cnt2][13], sizeof(char) * 7, "%06X", full_len / 2);
							break;
						}
					}
					_obcode[0] = 'E';
					full_len = 0;
					strcpy_s(now_section, strlen(token_table[cnt]->label) + 1, token_table[cnt]->label);
					object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
					strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
					ob_line_num++;
					
					

					char _obcode[70] = { 0, };
					_obcode[0] = 'H'; // 1��°�� H���ڵ� ǥ��
					strcpy_s(&_obcode[1], strlen(token_table[cnt]->label) + 1, token_table[cnt]->label); // ���α׷� �̸� ����
					for (cnt2 = strlen(token_table[cnt]->label) + 1; cnt2 < 7; cnt2++) { // �����ڸ� ����
						_obcode[cnt2] = ' ';
					}
					_obcode[19] = '\0';
					sprintf_s(&_obcode[7], 7, "%06X", 0);
					sprintf_s(&_obcode[13], 7, "000000");
					object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
					strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
					ob_line_num++;
					continue;
				}

			}

			object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
			strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
			ob_line_num++;
		}
		else if (strcmp(token_table[cnt]->operator_, "LTORG") == 0 ||
			strcmp(token_table[cnt]->operator_, "EQU") == 0) {
			continue;
		}
		else {
			int t_len = 0;

			int end_idx = 0;

			_obcode[0] = 'T';

			sprintf_s(&_obcode[1], sizeof(char) * 7, "%06X", full_len / 2);
			_obcode[7] = _obcode[8] = '0';
			_obcode[9] = '\0';
			int cnt2 = 0;
			for (cnt2 = cnt; ; cnt2++, cnt++) {
				if (token_table[cnt2] == NULL) {
					break;
				}
				else if (strcmp(token_table[cnt]->operator_, "RESB") == 0) {
					full_len += (atoi(token_table[cnt]->operand[0]) * sizeof(char) * 2);
					continue;
				}
				else if (strcmp(token_table[cnt]->operator_, "RESW") == 0) {
					full_len += (atoi(token_table[cnt]->operand[0]) * sizeof(char) * 3 * 2);
					continue;
				}
				else {
					if (token_table[cnt2]->obcode != NULL) {
						if (strlen(token_table[cnt2]->obcode) + t_len <= 60) {
							if (strlen(_obcode) == 9) {
								char addr[5];
								sprintf_s(addr, sizeof(char) * 5, "%04X", token_table[cnt2]->addr);
								_obcode[3] = addr[0];
								_obcode[4] = addr[1];
								_obcode[5] = addr[2];
								_obcode[6] = addr[3];
							}
							strcpy_s(&_obcode[strlen(_obcode)], strlen(token_table[cnt2]->obcode) + 1, token_table[cnt2]->obcode);
							int sym = search_symbol(token_table[cnt2]->operand[0], now_section);
							if (sym >= 0 && token_table[cnt2]->operator_[0] == '+' ||
								(strcmp(token_table[cnt2]->operator_, "EQU") != 0 && (
									strchr(token_table[cnt2]->operand[0], '+') != NULL ||
									strchr(token_table[cnt2]->operand[0], '-') != NULL ||
									strchr(token_table[cnt2]->operand[0], '*') != NULL ||
									strchr(token_table[cnt2]->operand[0], '/') != NULL ||
									strchr(token_table[cnt2]->operand[0], '%') != NULL))) { // m_rec
								char m_record[17] = { 0, };
								m_record[0] = 'M';
								if (token_table[cnt2]->operator_[0] == '+') {
									sprintf_s(&m_record[1], sizeof(char) * 7, "%06X", full_len / 2 + 1);
									sprintf_s(&m_record[7], sizeof(char) * 3, "%02X", (full_len + strlen(token_table[cnt2]->obcode) - full_len + 4) / 2 - 1);
								}
								else {
									sprintf_s(&m_record[1], sizeof(char) * 7, "%06X", full_len / 2);
									sprintf_s(&m_record[7], sizeof(char) * 3, "%02X", (full_len + strlen(token_table[cnt2]->obcode) - full_len + 4) / 2 + 1);
								}
								char *deli;
								char cpy[20];
								strcpy_s(cpy, strlen(token_table[cnt2]->operand[0]) + 1, token_table[cnt2]->operand[0]);
								if (strchr(cpy, '+') != NULL) {
									deli = strchr(cpy, '+');
								}
								else if (
									strchr(cpy, '-') != NULL) {
									deli = strchr(cpy, '-');
								}
								else if (
									strchr(cpy, '*') != NULL) {
									deli = strchr(cpy, '*');
								}
								else if (
									strchr(cpy, '/') != NULL) {
									deli = strchr(cpy, '/');
								}
								else if (
									strchr(cpy, '%') != NULL)
								{
									deli = strchr(cpy, '%');
								}
								else {
									deli = NULL;
								}
								if (deli != NULL) {
									char m_record_tmp[17];
									strcpy_s(m_record_tmp, strlen(m_record) + 1, m_record);
									strcat_s(m_record_tmp, strlen(m_record_tmp) + strlen(deli) + 1, deli);
									*deli = '\0';
									strcat_s(m_record, strlen(m_record) + 2, "+");
									strcat_s(m_record, strlen(m_record) + strlen(cpy) + 1, cpy);

								
									object_codes[ob_line_num] = (char *)malloc(strlen(m_record) + 1);
									strcpy_s(object_codes[ob_line_num], strlen(m_record) + 1, m_record);
									ob_line_num++;
									object_codes[ob_line_num] = (char *)malloc(strlen(m_record_tmp) + 1);
									strcpy_s(object_codes[ob_line_num], strlen(m_record_tmp) + 1, m_record_tmp);
									ob_line_num++;

								}
								else {
									
									strcat_s(m_record, strlen(m_record) + 2, "+");
									strcat_s(m_record, strlen(m_record) + strlen(token_table[cnt2]->operand[0]) + 1, token_table[cnt2]->operand[0]);
									object_codes[ob_line_num] = (char *)malloc(strlen(m_record) + 1);
									strcpy_s(object_codes[ob_line_num], strlen(m_record) + 1, m_record);
									ob_line_num++;
								}
							}
							t_len += strlen(token_table[cnt2]->obcode);
							full_len += strlen(token_table[cnt2]->obcode);
						}
						else {
							break;
						}
					}
					else {
						break;
					}
				}
			}
			char tmp[3];
			sprintf_s(tmp, sizeof(char) * 3, "%02X", t_len / 2);
			_obcode[7] = tmp[0];
			_obcode[8] = tmp[1];
			object_codes[ob_line_num] = (char *)malloc(strlen(_obcode) + 1);
			strcpy_s(object_codes[ob_line_num], strlen(_obcode) + 1, _obcode);
			ob_line_num++;
			cnt--;
		}
	}
	int cnt2 = 0;
	int cnt3 = 0;
	char *tmp;

	for (cnt = ob_line_num - 1; cnt >= 0; cnt--) {
		if (object_codes[cnt][0] == 'T') {
			for (cnt2 = cnt - 1; cnt2 >= 0; cnt2--) {
				if (object_codes[cnt2][0] == 'M') {
					for (cnt3 = cnt2; cnt3 < cnt; cnt3++) {
						tmp = object_codes[cnt3];
						object_codes[cnt3] = object_codes[cnt3 + 1];
						object_codes[cnt3 + 1] = tmp;
					}
					break;
				}
				else if (object_codes[cnt2][0] == 'E') {
					cnt = cnt2;
					break;
				}
			}
		}
	}
	for (cnt = ob_line_num -1; cnt >= 0; cnt--) {
		if (object_codes[cnt][0] == 'E') {
			if (strlen(object_codes[cnt]) > 1) {
				for (cnt2 = 0; cnt2 < cnt; cnt2++) {
					if (object_codes[cnt2][0] == 'E') {
						tmp = object_codes[cnt2];
						object_codes[cnt2] = object_codes[cnt];
						object_codes[cnt] = tmp;
						break;
					}
				}
			}
			break;
		}
	}
	for (cnt = 0; object_codes[cnt] != NULL; cnt++) {
		printf("%s\n", object_codes[cnt]);
		if (object_codes[cnt][0] == 'E')
			printf("\n");

	}
	printf("\n");
	return;
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
		strcmp(token_table[token_line]->operator_, "END") != 0 &&
		strcmp(token_table[token_line]->operator_, "LTORG") != 0 &&
		strcmp(token_table[token_line]->operator_, "EQU") != 0 &&
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
	else if (strcmp(token_table[token_line]->operator_, "EQU") == 0) {
		if (strcmp(token_table[token_line]->operand[0], "*") == 0) {
			token_table[token_line]->addr = locctr;
		}
		else {
			int answer = get_calculated_operand(token_line, now_section);
			if (answer > 0) {
				token_table[token_line]->addr = answer;
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
	int cnt = 0;

	// ���� ���ǿ� ���� �ɺ��̸��� ������ ��� address ����. 
	for (cnt = 0; cnt < sym_num; cnt++) {
		if (strcmp(sym_table[cnt]->symbol, name) == 0)
			if (strcmp(sym_table[cnt]->section, section) == 0) {
				sym_table[cnt]->addr = address;
				return;
			}
	}


	sym_table[sym_num] = (struct symbol_unit*)malloc(sizeof(struct symbol_unit));
	sym_table[sym_num]->addr = address;
	strcpy_s(sym_table[sym_num]->symbol, strlen(name) + 1, name);
	strcpy_s(sym_table[sym_num]->section, strlen(section) + 1, section);
	sym_num++;
}

/* -----------------------------------------------------------------------------------
* ���� : ���۷��忡 ������ �ִ� ��� ����ϴ� �Լ�.
* �Ű� : token_table�� �ε���, ���� ����
* ��ȯ : ���� ��.
*
* -----------------------------------------------------------------------------------
*/
int get_calculated_operand(int line, char* section) {
	int cnt = 0;
	for (cnt = 0; (cnt < MAX_OPERAND) && (strlen(token_table[line]->operand[cnt]) > 0); cnt++) {
		char *op = 0;
		char cpy_str[20];
		char first_symbol[10];
		char second_symbol[10];
		strcpy_s(cpy_str, strlen(token_table[line]->operand[cnt]) + 1, token_table[line]->operand[cnt]);
		if (op = strchr(cpy_str, '+')) {
			strcpy_s(second_symbol, strlen(op), op + 1);
			*op = '\0';
			strcpy_s(first_symbol, strlen(cpy_str) + 1, cpy_str);
			return (sym_table[search_symbol(first_symbol, section)]->addr) + (sym_table[search_symbol(second_symbol, section)]->addr);
		}
		else if (op = strchr(cpy_str, '-')) {
			strcpy_s(second_symbol, strlen(op), op + 1);
			*op = '\0';
			strcpy_s(first_symbol, strlen(cpy_str) + 1, cpy_str);
			return (sym_table[search_symbol(first_symbol, section)]->addr) - (sym_table[search_symbol(second_symbol, section)]->addr);
		}
		else if (op = strchr(cpy_str, '*')) {
			strcpy_s(second_symbol, strlen(op), op + 1);
			*op = '\0';
			strcpy_s(first_symbol, strlen(cpy_str) + 1, cpy_str);
			return (sym_table[search_symbol(first_symbol, section)]->addr) * (sym_table[search_symbol(second_symbol, section)]->addr);
		}
		else if (op = strchr(cpy_str, '/')) {
			strcpy_s(second_symbol, strlen(op), op + 1);
			*op = '\0';
			strcpy_s(first_symbol, strlen(cpy_str) + 1, cpy_str);
			return (sym_table[search_symbol(first_symbol, section)]->addr) / (sym_table[search_symbol(second_symbol, section)]->addr);
		}
		else if (op = strchr(cpy_str, '%')) {
			strcpy_s(second_symbol, strlen(op), op + 1);
			*op = '\0';
			strcpy_s(first_symbol, strlen(cpy_str) + 1, cpy_str);
			return (sym_table[search_symbol(first_symbol, section)]->addr) % (sym_table[search_symbol(second_symbol, section)]->addr);
		}
	}
	return -1;
}
/* -----------------------------------------------------------------------------------
* ���� : �ɺ� ���̺��� �ɺ��� �˻��ϴ� �Լ�.
* �Ű� : �ɺ��̸�, �˻��� �����̸�
* ��ȯ : �ش� �ɺ��� ���ٸ� -1, �ִٸ� �ش� �ɺ��� ���̺� �ε���.
*
* -----------------------------------------------------------------------------------
*/

int search_symbol(char *name, char *section) {
	int cnt = 0;
	for (cnt = 0; cnt < sym_num; cnt++) {
		if (strcmp(name, sym_table[cnt]->symbol) == 0 && strcmp(section, sym_table[cnt]->section) == 0) {
			return cnt;
		}
	}
	return -1;
}

/* -----------------------------------------------------------------------------------
* ���� :
* �Ű� :
* ��ȯ :
*
* -----------------------------------------------------------------------------------
*/

int get_register_num(char *name) {
	if (strcmp(name, "A") == 0) {
		return 0;
	}
	else if (strcmp(name, "X") == 0) {
		return 1;
	}
	else if (strcmp(name, "L") == 0) {
		return 2;
	}
	else if (strcmp(name, "B") == 0) {
		return 3;
	}
	else if (strcmp(name, "S") == 0) {
		return 4;
	}
	else if (strcmp(name, "T") == 0) {
		return 5;
	}
	else if (strcmp(name, "F") == 0) {
		return 6;
	}
	else if (strcmp(name, "PC") == 0) {
		return 8;
	}
	else if (strcmp(name, "SW") == 0) {
		return 9;
	}
	else {
		return -1;
	}
}



/* -----------------------------------------------------------------------------------
* ���� :
* �Ű� :
* ��ȯ :
*
* -----------------------------------------------------------------------------------
*/