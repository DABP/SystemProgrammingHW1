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
struct inst_unit {			// instruction�� �����ϴ� ����ü�̴�.
	char *name;				// instruction�� �̸� (ex. ADD)
	unsigned char opcode;	// OPCODE (ex. 18)
	int format;				// Format (1 ~ 4)
	int operand_num;		// �ǿ����� ����(0 ~ 3)
};
typedef struct inst_unit inst;	// inst�� ������
inst *inst_table[MAX_INST];		// inst_table ����ü ������ ����
int inst_index;					// inst_table ����ü�� �� ����

/*
* ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
*/
char *input_data[MAX_LINES];	// �ҽ��ڵ带 Line ������ �����Ѵ�.
static int line_num;			// input_data�� ���� ����

int label_num;					// label�� ����

/*
* ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
* operator�� renaming�� ����Ѵ�.
* nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
*/
struct token_unit {				// input_data�� ���� ������ ����� �ҽ��ڵ带 �Ľ��Ͽ� �����Ѵ�.
	int addr;					// ��� �ּҰ�. (+= start address) 16����.
	char *label;				// �� ����.
	char *operator_;			// operator ���� (�����, instruction)
	char *operand[MAX_OPERAND];	// operand ���� (�ε����� 0���� 2���� �� 3�� �����ϴ�.)
	char *comment;				// comment ����.
	char *obcode;
	char nixbpe; // ���� ������Ʈ���� ���ȴ�.
};

typedef struct token_unit token;	// token���� ������.
token *token_table[MAX_LINES];		// token_table ����ü ������ ����.
static int token_line;				// token_table ����ü�� �� ����.

/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
*/
struct symbol_unit {		// �ɺ��� �����Ѵ�.
	char symbol[10];		// �̸�
	int addr;				// ��ġ
	char section[10];		// ����� �̸�.
};

typedef struct symbol_unit symbol;	// symbol�� ������.
symbol *sym_table[MAX_LINES];		// sym_table�� ����.
int sym_num;						// sym_table�� ���� ��.
char now_section[10];				// ���� ���� �̸�.
/*
* ���ͷ� ����� �ӽ÷� �����ϰ� �ִ� ���̺��̴�.
*/
struct literal_pool {
	char *name;
};
typedef struct literal_pool literal_p;	
literal_p *literal_table[MAX_LINES];		// literal_table ����.
int literal_num;						// literal_table row counter

static int locctr;		// LOCATION COUNTER

char *object_codes[70];	// full object codes.
int ob_line_num;		// object code line number.
//--------------

static char *input_file;						// input.txt�� ����������
static char *output_file;						// output�� ���� ������
int init_my_assembler(void);					// input.txt�� inst.data�� �ʱ�ȭ �ϴ� �Լ�
int init_inst_file(char *inst_file);			// inst.data �Է� �� �Ľ�
int init_input_file(char *input_file);			// input.txt �Է� �� ����.
int search_opcode(char *str);					// instruction ���� �˻� �� �ش� line return 
void make_opcode_output(char *file_name);		// ����� ���
int token_parsing(int index);					// input.txt�κ��� �о���� ������ ����ü�� parsing�Ͽ� ����.
/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
static int assem_pass1(void);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);

/* �߰� �Լ� ���� */
void initialize_label(int index);					// label�� �ּһ������ �ʱ�ȭ
void initialize_operand(int index, int start_num);	// operand�� �ּ� ������� ���� �ʱ�ȭ
void initialize_comment(int index);					// comment�� �ּ� ������� �ʱ�ȭ
void set_location_counter();						// ���κ� location counter �ʱ�ȭ
void set_literal(char* str);						// literal line ����.
int def_literal();
void add_symbol(int, char *, char *);
int get_calculated_operand(int line, char* section);
int search_symbol(char *name, char *section);
int get_register_num(char *name);
void fill_zero(int line, char* answer);