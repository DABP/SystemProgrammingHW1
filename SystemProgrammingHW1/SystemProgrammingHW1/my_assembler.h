/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
*
*/
#define MAX_INST 256		// ��ɾ� �ִ� ����
#define MAX_LINES 5000		// ���α׷� �ڵ��� �ִ� �� �� 
#define MAX_OPERAND 3		// �� operator���� ���� �� �ִ� operand�� �ִ� ����

/*
* instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
* ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
* ���� ���� �ϳ��� instruction�� �����Ѵ�.
*/
struct inst_unit {
	/* add your code here */
	char *name;
	unsigned char opcode;
	int format;
	int operand_num;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
* ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
* operator�� renaming�� ����Ѵ�.
* nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
*/
struct token_unit {
	char *label;
	char *operator_;
	char *operand[MAX_OPERAND];
	char *comment;
	//char nixbpe; // ���� ������Ʈ���� ���ȴ�.

};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int search_opcode(char *str);
void make_opcode_output(char *file_name);
int token_parsing(int index);
/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
static int assem_pass1(void);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);

/* �߰� �Լ� ���� */
void initialize_label(int index);
void initialize_operand(int index, int start_num);
void initialize_comment(int index);
