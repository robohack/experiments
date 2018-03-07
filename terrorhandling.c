enum {
	SUCCESS,
	EFAILED,
	EFOO,
	EBAR,
	ENUTS,
};

/*
 * such clarity, much simplicity!
 */
int
bar_goto_multi_error_boilerplate_is_much_shorter()
{
	void *res1 = NULL;		/* xxx must be init to NULL */
	void *res2 = NULL;
	void *res3 = NULL;
	int result = EFAILED;

	if (pre_flight_check_fails()) {
		goto get_out;
	}

	if (alloc(&res1)) {
		goto get_out;
	}

	if (alloc(&res2)) {
		goto get_out;
	}

	if (alloc(&res3)) {
		goto get_out;
	}

	/*
	 *	do tasks with resources...
	 */
	if (task_failed()) {
		goto get_out;
	}

	/* everything worked! */
	result = SUCCESS;

    get_out:

/*
 * hmmmm....  What about using a statement expressino instead to allow the free
 * function to return a result code?
 *
 * ({int _x; (p) ? (_x=f(p), p=NULL, _x) : SUCCESS})
 */
#define FREE_CALL(f, p) do { if (p != NULL) { f(p); p = NULL; } } while(0)

	FREE_CALL(free, res3);
	FREE_CALL(free, res2);
	FREE_CALL(free, res1);

	return result;
}

/*
 * no goto's, such complexity!
 */
int
bar_no_gotos_or_macros_multi_error_boilerplate()
{
	enum err_cln_e {
		RES0,
		RES1,
		RES2,
		RES3,
	} error_cleanup;
	bool failed_allocs;
	bool failed_tasks;
	/*
	 * above are structure-related variables assisting error handling
	 *
	 * the rest are allocated resource-related variables that need full
	 * scope in the function (used in the error handling "switch" below)
	 */
	void *res1, *res2, *res3;

	failed_allocs = true;
	failed_tasks = true;
	error_cleanup = RES0;
	/*
	 * everything the function does is in a "do{}while(false)"
	 * one-hit-wonder so that a single "break" statement will drop out to
	 * the error handling "switch" statement below
	 */
	do {
		/* any non-resource-related local variables are declared here */

		if (pre_flight_check_fails()) {
			break;
		}

		if (alloc(&res1)) {
			error_cleanup = RES1;
		} else {
			break;
		}

		if (alloc(&res2)) {
			error_cleanup = RES2;
		} else {
			break;
		}

		if (alloc(&res3)) {
			error_cleanup = RES3;
		} else {
			break;
		}
		failed_allocs = false;

		/*
		 *	do tasks with resources...
		 */
		if (task_failed()) {
			break;
		}

		/* everything worked! */
		failed_tasks = false;
	} while (false);
	switch (error_cleanup) {
	case RES3:
		free(res3);
		/* FALLTHROUGH */
	case RES2:
		free(res2);
		/* FALLTHROUGH */
	case RES1:
		free(res1);
		/* FALLTHROUGH */
	case RES0:
		if (failed_allocs) {
			/* pre-flight or resource setup error */
		}
		if (failed_tasks) {
			/* pre-flight or execute error */
		}
		break;
	default:
		panic("I didn't expect this one.");
		/* NOTREACHED, or set failed_tasks = true */
	}

	return (failed_allocs || failed_tasks) ? EFAILED : SUCCESS;
}

/*
 * By making use of the invariant that all arguments that are asked for must be
 * received, explicit responsibility for handling the exception is enforced
 * upon the calling code.
 *
 * This enforced responsibility can work its way up as far as necessary.
 *
 * Condition handlers are functions or objects which can be defined externally
 * to client code.  This helps decouple the client code from the error handling
 * code, and also improves cohesion.
 *
 * xxx but it is very hard to read and comprehend
 */
bool
condition_two_handler()
{
	if (i_can_fix_condition_two) {
		fix_condition_two;

		return false;
	}

	return true;			/* failed... */
}

bool
condition_one_handler(resources)
{
	if (i_can_fix_condition_one) {
		fix_condition_one;

		return false;
	}

	free(resources);

	return true;			/* failed... */
}

typedef int error_code_t;

error_code_t
some_fn(condition_two_handler)
{
	error_code_t error = SUCCESS;

	if (something_is_not_ok()) {
 		error = ENUTS;
	}

	return error;
}

error_code_t
some_fn_with_resources(resources, condition_one_handler, condition_two_handler)
{
	error_code_t error = OK;

	if (error_condition_one() && condition_one_handler(resources)) {
		return EFOO;
	}
	if (error_condition_two() && condition_two_handler()) {
		return EBAR;
	}
	execute_code(resources);

	return SUCCESS;
}

error_code_t
without_resources(some_fn, condition_two_handler)
{
	error_code_t error;

	error = some_fn(condition_two_handler);

	return error;
}

error_code_t
with_resources(some_fn_with_resources, condition_one_handler)
{
	void *resources;
	error_code_t error;

	resources = set_up_resources();
	error = some_fn_with_resources(resources, condition_one_handler, condition_two_handler);
	release_resources();

	return error;
}

int
main()
{
	error_code_t error;

	do {
		error = with_resources(some_fn_with_resources, condition_one_handler);
		if (error) {
			break;
		}
		error = without_resources(some_fn, condition_two_handler);
		if (error) {
			break;
		}
	} while (false);

	exit(error ? EXIT_FAILED : EXIT_SUCCESS);
}
