void
foo()
{
	int             which = DBSETBCP;

	switch (which) {
	case DBSETBCP:
		tds_set_bulk(login->tds_login, (TDS_TINYINT) value);
		return SUCCEED;
		break;
	case DBSETNOSHORT:
	case DBSETENCRYPT:
	case DBSETLABELED:
	}
}
