/*
* my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
*
*/
#define MAX_INST 256		// 명령어 최대 개수
#define MAX_LINES 5000		// 프로그램 코드의 최대 줄 수 

#define MAX_COLUMNS 4		// label, operator, operand, comment
#define MAX_OPERAND 3		// 한 operator에서 가질 수 있는 operand의 최대 개수

/*
* 기계어 목록 파일로 부터 정보를 받아와 생성하는 기계어 변환 테이블이다.
* 해당 기계어의 정보를 토큰으로 나눠 기록하고 있다.
*/
struct inst_struct {
	char *str;
	unsigned char op;
	int format;
	int ops;
};
typedef struct inst_struct inst_struct;
inst_struct *inst[MAX_INST];
int inst_index;

/*
* 어셈블리 할 소스코드를 토큰 단위로 관리하는 테이블이다.
* 관리 정보는 소스 라인 단위로 관리되어진다.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

struct token_unit {
	char *label;
	char *operator_;
	char *operand[MAX_OPERAND];
	char *comment;
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;



/*
* 심볼을 관리하는 구조체이다.
* 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
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
static int assem_pass1(void);
static int assem_pass2(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int search_opcode(char *str);
void make_objectcode(char *file_name);
void initialize_label(int);
void initialize_operand(int, int);
void initialize_comment(int);