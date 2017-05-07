/*
* my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
*
*/
#define MAX_INST 256		// 명령어 최대 개수
#define MAX_LINES 5000		// 프로그램 코드의 최대 줄 수 
#define MAX_OPERAND 3		// 한 operator에서 가질 수 있는 operand의 최대 개수

/*
* instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
* 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
* 라인 별로 하나의 instruction을 저장한다.
*/
struct inst_unit {			// instruction을 관리하는 구조체이다.
	char *name;				// instruction의 이름 (ex. ADD)
	unsigned char opcode;	// OPCODE (ex. 18)
	int format;				// Format (1 ~ 4)
	int operand_num;		// 피연산자 개수(0 ~ 3)
};
typedef struct inst_unit inst;	// inst로 재정의
inst *inst_table[MAX_INST];		// inst_table 구조체 포인터 선언
int inst_index;					// inst_table 구조체의 총 개수

/*
* 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
*/
char *input_data[MAX_LINES];	// 소스코드를 Line 단위로 관리한다.
static int line_num;			// input_data의 라인 개수

int label_num;					// label의 개수

/*
* 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
* operator는 renaming을 허용한다.
* nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
*/
struct token_unit {				// input_data에 라인 단위로 저장된 소스코드를 파싱하여 저장한다.
	int addr;					// 상대 주소값. (+= start address) 16진수.
	char *label;				// 라벨 저장.
	char *operator_;			// operator 저장 (예약어, instruction)
	char *operand[MAX_OPERAND];	// operand 저장 (인덱스는 0부터 2까지 총 3개 가능하다.)
	char *comment;				// comment 저장.
	char *obcode;
	char nixbpe; // 추후 프로젝트에서 사용된다.
};

typedef struct token_unit token;	// token으로 재정의.
token *token_table[MAX_LINES];		// token_table 구조체 포인터 선언.
static int token_line;				// token_table 구조체의 총 개수.

/*
* 심볼을 관리하는 구조체이다.
* 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
*/
struct symbol_unit {		// 심볼을 관리한다.
	char symbol[10];		// 이름
	int addr;				// 위치
	char section[10];		// 제어섹션 이름.
};

typedef struct symbol_unit symbol;	// symbol로 재정의.
symbol *sym_table[MAX_LINES];		// sym_table을 생성.
int sym_num;						// sym_table의 라인 수.
char now_section[10];				// 현재 섹션 이름.
/*
* 리터럴 상수를 임시로 저장하고 있는 테이블이다.
*/
struct literal_pool {
	char *name;
};
typedef struct literal_pool literal_p;	
literal_p *literal_table[MAX_LINES];		// literal_table 생성.
int literal_num;						// literal_table row counter

static int locctr;		// LOCATION COUNTER

char *object_codes[70];	// full object codes.
int ob_line_num;		// object code line number.
//--------------

static char *input_file;						// input.txt의 파일포인터
static char *output_file;						// output의 파일 포인터
int init_my_assembler(void);					// input.txt와 inst.data를 초기화 하는 함수
int init_inst_file(char *inst_file);			// inst.data 입력 및 파싱
int init_input_file(char *input_file);			// input.txt 입력 및 저장.
int search_opcode(char *str);					// instruction 인지 검사 및 해당 line return 
void make_opcode_output(char *file_name);		// 결과를 출력
int token_parsing(int index);					// input.txt로부터 읽어들인 데이터 구조체에 parsing하여 넣음.
/* 추후 프로젝트에서 사용하게 되는 함수*/
static int assem_pass1(void);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);

/* 추가 함수 선언 */
void initialize_label(int index);					// label을 최소사이즈로 초기화
void initialize_operand(int index, int start_num);	// operand를 최소 사이즈로 각각 초기화
void initialize_comment(int index);					// comment를 최소 사이즈로 초기화
void set_location_counter();						// 라인별 location counter 초기화
void set_literal(char* str);						// literal line 생성.
int def_literal();
void add_symbol(int, char *, char *);
int get_calculated_operand(int line, char* section);
int search_symbol(char *name, char *section);
int get_register_num(char *name);
void fill_zero(int line, char* answer);