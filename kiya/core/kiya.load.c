struct kiya_t;
struct pocket_s;

struct kiya_t* kiya;
struct pocket_s* (*kiya$load$kiya)(struct pocket_s *restrict pocket, struct kiya_t *restrict kiya);
struct pocket_s* (*kiya$load$kiiyaa)(struct pocket_s *restrict pocket, struct kiya_t *restrict kiya);
struct pocket_s* (*kiya$load$source)(struct pocket_s *restrict pocket, struct kiya_t *restrict kiya);

struct pocket_s* kiya$tag$kiya(struct pocket_s *restrict pocket)
{
	return kiya$load$kiya(pocket, kiya);
}

struct pocket_s* kiya$tag$kiiyaa(struct pocket_s *restrict pocket)
{
	return kiya$load$kiiyaa(pocket, kiya);
}

struct pocket_s* kiya$tag$source(struct pocket_s *restrict pocket)
{
	return kiya$load$source(pocket, kiya);
}
