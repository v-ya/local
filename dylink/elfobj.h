#ifndef _elfobj_h_
#define _elfobj_h_

#include <stdint.h>
#include <sys/types.h>

typedef struct elf64obj_t elf64obj_t;

elf64obj_t* elf64obj_load(const char *elf64obj_path);
void elf64obj_free(elf64obj_t* e);
void elf64obj_delete_sym(elf64obj_t *e, const char *name);
uint8_t* elf64obj_build_strpool(elf64obj_t *e, size_t *psize);
uint8_t* elf64obj_build_image(elf64obj_t *e, size_t *psize, size_t *ptakeup);
int elf64obj_build_import(elf64obj_t *e);
void elf64obj_build_export(elf64obj_t *e);
int elf64obj_build_link_self(elf64obj_t *e);
uint8_t* elf64obj_build_dylink(elf64obj_t *e, size_t *psize);
void elf64obj_dump_session(elf64obj_t* e);
void elf64obj_dump_symtab(elf64obj_t* e);
void elf64obj_dump_rela(elf64obj_t* e);
void dylink_dump(uint8_t *r, size_t size);

#endif
