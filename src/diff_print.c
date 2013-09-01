	if (!diff || !diff->repo)
		pi->oid_strlen = GIT_ABBREV_DEFAULT;
	else if (git_repository__cvar(
		&pi->oid_strlen, diff->repo, GIT_CVAR_ABBREV) < 0)
static char diff_pick_suffix(int mode)
	else if (mode & 0100) /* -V536 */
static int diff_print_one_compact(
	git_buf *out = pi->buf;
	int (*strcomp)(const char *, const char *) =
		pi->diff ? pi->diff->strcomp : git__strcmp;
	old_suffix = diff_pick_suffix(delta->old_file.mode);
	new_suffix = diff_pick_suffix(delta->new_file.mode);
	git_buf_clear(out);
		strcomp(delta->old_file.path,delta->new_file.path) != 0)
		git_buf_printf(out, "%c\t%s%c %s%c\n", code,
		git_buf_printf(out, "%c\t%s%c %s%c\n", code,
			delta->old_file.path, old_suffix, delta->new_file.path, new_suffix);
		git_buf_printf(out, "%c\t%s%c\n", code, delta->old_file.path, old_suffix);
		git_buf_printf(out, "%c\t%s\n", code, delta->old_file.path);
	if (git_buf_oom(out))
			git_buf_cstr(out), git_buf_len(out), pi->payload))
/* print a git_diff_list to a print callback in compact format */
		error = git_diff_foreach(diff, diff_print_one_compact, NULL, NULL, &pi);
static int diff_print_one_raw(
	git_buf *out = pi->buf;
	git_buf_clear(out);
		out, ":%06o %06o %s... %s... %c",
		git_buf_printf(out, "%03u", delta->similarity);
	if (delta->old_file.path != delta->new_file.path)
			out, "\t%s %s\n", delta->old_file.path, delta->new_file.path);
			out, "\t%s\n", delta->old_file.path ?
	if (git_buf_oom(out))
			git_buf_cstr(out), git_buf_len(out), pi->payload))
/* print a git_diff_list to a print callback in raw output format */
		error = git_diff_foreach(diff, diff_print_one_raw, NULL, NULL, &pi);
static int diff_print_oid_range(
	git_buf *out, const git_diff_delta *delta, int oid_strlen)
	git_oid_tostr(start_oid, oid_strlen, &delta->old_file.oid);
	git_oid_tostr(end_oid, oid_strlen, &delta->new_file.oid);
		git_buf_printf(out, "index %s..%s %o\n",
			git_buf_printf(out, "new file mode %o\n", delta->new_file.mode);
			git_buf_printf(out, "deleted file mode %o\n", delta->old_file.mode);
			git_buf_printf(out, "old mode %o\n", delta->old_file.mode);
			git_buf_printf(out, "new mode %o\n", delta->new_file.mode);
		git_buf_printf(out, "index %s..%s\n", start_oid, end_oid);
	if (git_buf_oom(out))
static int diff_delta_format_with_paths(
	git_buf *out,
	const git_diff_delta *delta,
	const char *oldpfx,
	const char *newpfx,
	const char *template)
	if (git_oid_iszero(&delta->old_file.oid)) {
		oldpfx = "";
		oldpath = "/dev/null";
	}
	if (git_oid_iszero(&delta->new_file.oid)) {
		newpfx = "";
		newpath = "/dev/null";
	}
	return git_buf_printf(out, template, oldpfx, oldpath, newpfx, newpath);
}
int git_diff_delta__format_file_header(
	git_buf *out,
	const git_diff_delta *delta,
	const char *oldpfx,
	const char *newpfx,
	int oid_strlen)
{
	if (!oid_strlen)
		oid_strlen = GIT_ABBREV_DEFAULT + 1;
	git_buf_clear(out);
	git_buf_printf(out, "diff --git %s%s %s%s\n",
		oldpfx, delta->old_file.path, newpfx, delta->new_file.path);

	if (diff_print_oid_range(out, delta, oid_strlen) < 0)
	if ((delta->flags & GIT_DIFF_FLAG_BINARY) == 0)
		diff_delta_format_with_paths(
			out, delta, oldpfx, newpfx, "--- %s%s\n+++ %s%s\n");
	return git_buf_oom(out) ? -1 : 0;
}
static int diff_print_patch_file(
	const git_diff_delta *delta, float progress, void *data)
{
	diff_print_info *pi = data;
	const char *oldpfx =
		pi->diff ? pi->diff->opts.old_prefix : DIFF_OLD_PREFIX_DEFAULT;
	const char *newpfx =
		pi->diff ? pi->diff->opts.new_prefix : DIFF_NEW_PREFIX_DEFAULT;
	uint32_t opts_flags = pi->diff ? pi->diff->opts.flags : GIT_DIFF_NORMAL;

	GIT_UNUSED(progress);

	if (S_ISDIR(delta->new_file.mode) ||
		delta->status == GIT_DELTA_UNMODIFIED ||
		delta->status == GIT_DELTA_IGNORED ||
		(delta->status == GIT_DELTA_UNTRACKED &&
		 (opts_flags & GIT_DIFF_INCLUDE_UNTRACKED_CONTENT) == 0))
		return 0;

	if (git_diff_delta__format_file_header(
			pi->buf, delta, oldpfx, newpfx, pi->oid_strlen) < 0)

	if (diff_delta_format_with_paths(
			pi->buf, delta, oldpfx, newpfx,
			"Binary files %s%s and %s%s differ\n") < 0)
static int diff_print_patch_hunk(
static int diff_print_patch_line(
/* print a git_diff_list to an output callback in patch format */
			diff, diff_print_patch_file, diff_print_patch_hunk,
			diff_print_patch_line, &pi);
/* print a git_diff_patch to an output callback */
			patch, diff_print_patch_file, diff_print_patch_hunk,
			diff_print_patch_line, &pi);
static int diff_print_to_buffer_cb(
	const git_diff_delta *delta,
	const git_diff_range *range,
	char line_origin,
	const char *content,
	size_t content_len,
	void *payload)
{
	git_buf *output = payload;
	GIT_UNUSED(delta); GIT_UNUSED(range); GIT_UNUSED(line_origin);
	return git_buf_put(output, content, content_len);
}

/* print a git_diff_patch to a string buffer */
	error = git_diff_patch_print(patch, diff_print_to_buffer_cb, &output);