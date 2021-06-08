DB_DIFF_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source $DB_DIFF_PATH"/../shared/include.sh"

trim()
{
    trimmed=$1
    trimmed=${trimmed%% }
    trimmed=${trimmed## }

    echo "$trimmed"
}

#
# Load configuration
#
source $DB_DIFF_PATH"/conf.dist"

if [ -f $DB_DIFF_PATH"/conf" ]; then
	source $DB_DIFF_PATH"/conf"
fi

#
# RUN SCRIPT
#

rm -R $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

op_list=["INSERT","UPDATE","REPLACE","DELETE"]

echo "Comparing databases..."

mysqldbcompare --server1=$DB_1_USER:$DB_1_PASS@$DB_1_HOST --server2=$DB_2_USER:$DB_2_PASS@$DB_2_HOST --difftype=sql $DB_1_NAME:$DB_2_NAME --run-all-tests > "$DB_DIFF_PATH/full.sql"

echo "Splitting files..."

new_tables=()

while IFS='' read -r line || [[ -n "$line" ]]
do
	first=${line:0:1}
	if [[ $first == "#" || $first == "" ]];then
	   if [[ ${line:0:16} == "#        TABLE: " ]];then
		   tablename=`trim ${line:15}`
		   new_tables+=($tablename)
		   mysqldump -u $DB_1_USER -p$DB_1_PASS --no-create-info --skip-comments --skip-set-charset --extended-insert=FALSE --complete-insert=TRUE --order-by-primary --single-transaction --quick $DB_2_NAME $tablename | sed -e 's/^\/\*![0-9]\{5\}.*\/;$//g' >> "$OUTPUT_DIR/$tablename.sql"
	   fi
	   continue;
	fi

	operation=${line%% *}
	name=${line#* \`$DB_1_NAME\`.\`}
	name=${name%%\` *}
	fline=${line/\`$DB_1_NAME\`.}
	if [ ! -z "$name" ]; then
		if [[ "${op_list[@]}" =~ "${operation}" && ! -z "$operation" ]]; then
			echo $fline >> $OUTPUT_DIR$name"___"${operation,,}".sql"
		else
			echo $line >> $OUTPUT_DIR"000-removed"
		fi
	fi
done < "full.sql"

echo "Sorting process..."

for f in $OUTPUT_DIR*
do
	LC_ALL=C sort -d -o $f $f
done

echo "Create schemas..."

for t in "${new_tables[@]}"
do
	mysqldump -u $DB_1_USER -p$DB_1_PASS --no-data --skip-comments --skip-set-charset --order-by-primary --single-transaction --quick $DB_2_NAME $t | sed -e 's/^\/\*![0-9]\{5\}.*\/;$//g' >> "$OUTPUT_DIR/000-$t-schema.sql"
done

#echo "Removing full.sql..."
#rm "$DB_DIFF_PATH/full.sql"
