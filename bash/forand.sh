# !/usr/bin/env bash

#Copyright 2022 Vulpes-Vulpeos
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#    http://www.apache.org/licenses/LICENSE-2.0
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

filterFileName()
{
	CURRENT_FILE=$1
	CURRENT_FILE_NAME="$(basename "${CURRENT_FILE}")"
	#move file to trash if cyrillic letter detected
	if [[ $CURRENT_FILE_NAME =~ [А-Яа-яЁё] ]]
	then
		mv "${CURRENT_FILE}" "${HOME}/.TRASH/$CURRENT_FILE_NAME"
	fi
}

unrandomizeFileName()
{
	CURRENT_FILE=$1
	CURRENT_FILE_DIR="$(dirname "${CURRENT_FILE}")/"
	CURRENT_FILE_NAME="$(basename "${CURRENT_FILE}")"

	if [[ "$CURRENT_FILE_NAME" == *". "* ]]
	then
		#remove prefix which ends with ". "
  		CURRENT_FILE_NAME=${CURRENT_FILE_NAME#*. }
  		mv "${CURRENT_FILE}" "$CURRENT_FILE_DIR$CURRENT_FILE_NAME"
	fi

	if [[ "$CURRENT_FILE_NAME" == *"_"* ]]
	then
		#remove prefix which ends with ". "
  		CURRENT_FILE_NAME=${CURRENT_FILE_NAME#*_}
  		mv "${CURRENT_FILE}" "$CURRENT_FILE_DIR$CURRENT_FILE_NAME"
	fi
}

randomizeFileName()
{
	CURRENT_FILE=$1
	NUMBER_OF_FILES=$2
	CURRENT_FILE_DIR="$(dirname "${CURRENT_FILE}")/"
	CURRENT_FILE_NAME="$(basename "${CURRENT_FILE}")"

	#Check if first character of file name is a number
	if [[ ${CURRENT_FILE_NAME::1} =~ [0-9] ]]
	then
		if [[ "$CURRENT_FILE_NAME" == *". "* ]]
		then
			#remove prefix which ends with ". "
  			CURRENT_FILE_NAME=${CURRENT_FILE_NAME#*. }
		fi

		if [[ "$CURRENT_FILE_NAME" == *"_"* ]]
		then
			#remove prefix which ends with ". "
  			CURRENT_FILE_NAME=${CURRENT_FILE_NAME#*_}
		fi
	fi

	#Generate random number and add it to file name
	RANDOM_NUMBER=$(( $RANDOM % ${NUMBER_OF_FILES} + 1 ))
	while [[ ${#RANDOM_NUMBER}<${#NUMBER_OF_FILES} ]]
	do
		RANDOM_NUMBER="0$RANDOM_NUMBER"
	done
	mv "${CURRENT_FILE}" "$CURRENT_FILE_DIR${RANDOM_NUMBER}_$CURRENT_FILE_NAME"
}

help ()
{
	echo "usage: [\"/path/to/forand.sh\"] [-fur] [\"/path/to/folder\"] ..."
	echo "-r  randomize files in folder."
	echo "-u  unrandomize files in folder."
	echo "-f  remove files which contain cyrillic letters in their file names."
	exit
}

if [[ $# < 2 ]]
then 
	echo "Provide at least 2 arguments: [-f | -u | -r | -ff | -fur etc.] \"path/to/diretory\""
	exit
fi

ARGUMENTS_ARRAY=()
#check if arguments start with -
if [[ ${1::1} == '-' ]]
then 
	ARGUMENTS=${1:1}
	#check if argument contains only f,u or r
	if [[ $ARGUMENTS =~ [FURfur] ]]
	then
		#split arguments string by characters and add them into array
		for (( i = 0; i < ${#ARGUMENTS}; i++ ))
		do
			ARGUMENTS_ARRAY[i]=${ARGUMENTS:i:1}
		done
		
	else
		help
	fi
else
	help
fi

#add all arguments except first to array
ARGUMENTS_PATHS_ARRAY=( "${@:2}" )
CHECKED_PATHS_ARRAY=()
#check if path exists
for (( i = 0; i < ${#ARGUMENTS_PATHS_ARRAY[@]}; i++ ))
do
	if [[ -d "${ARGUMENTS_PATHS_ARRAY[i]}" ]]
	then
		CHECKED_PATHS_ARRAY+=${ARGUMENTS_PATHS_ARRAY[i]}
	else
		echo "Path: $PATH_TO_ELEMENT do not exist."
	fi
done

#check if array with checked paths is empty
if (( ! ${#CHECKED_PATHS_ARRAY[@]} ))
	then
	echo "No existing paths were provided."
	exit	
fi

for (( i = 0; i < ${#CHECKED_PATHS_ARRAY[@]}; i++ ))
do
	#getting number of files in folder without hidden files
	#NUMBER_OF_FILES=$(find "${CHECKED_PATHS_ARRAY[i]}" ! -name '.*' -type f | wc -l)
	NUMBER_OF_FILES=$(find "${CHECKED_PATHS_ARRAY[i]}" -type f -not -path '*/\.*' | wc -l)
	NUMBER_OF_FILES=${NUMBER_OF_FILES//[[:blank:]]/}

	if [[ $NUMBER_OF_FILES == 0 ]]
	then
		echo "Folder is empty: ${CHECKED_PATHS_ARRAY[i]}"
		continue
	fi

	for (( j = 0; j < ${#ARGUMENTS_ARRAY[@]}; j++ ))
	do
		#getting path to file in a folder
		for CURRENT_FILE in "${CHECKED_PATHS_ARRAY[i]}"/*
		do
			case ${ARGUMENTS_ARRAY[j]} in
			f|F )
				filterFileName "$CURRENT_FILE"
				;;

			u|U )
				unrandomizeFileName "$CURRENT_FILE"
				;;

			r|R )
				#Check if first character of file name is a dot
				CURRENT_FILE_NAME="$(basename "${CURRENT_FILE}")"
				if [[ ${CURRENT_FILE_NAME::1} == '.' ]]
				then
					continue
				fi

				randomizeFileName "$CURRENT_FILE" "$NUMBER_OF_FILES"
				;;
			esac
		done		
	done
done
