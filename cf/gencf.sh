#!/bin/bash
MY_TEMP="tmp4324242.txt"
FOLDER=$PWD"/"
if [ $# -eq 2 ]; then
	if [ -e $2 ]; then 
		printf "Il file $2 esiste!!\nCambia il nome o eliminalo.\n" >&2
		exit 1
	fi
	if [ ! -x "./cf" ]; then 
		printf "L'eseguibile ./cf non esiste!!\n" >&2
		printf "esegui 'make' per compilarlo.\n" >&2
		exit 1
	fi
	START_TIME=`date +%s`
	if [ -e $MY_TEMP ]; then 
		rm $MY_TEMP
	fi
	FOLDER_COMUNI="$FOLDER""liste/comuni.txt"
	N_NOMI_M=$(cat -n $FOLDER"liste/nomi_m.txt" | wc -l)
	N_NOMI_M=$((N_NOMI_M + 1))
	N_NOMI_F=$(cat -n $FOLDER"liste/nomi_f.txt" | wc -l)
	N_NOMI_F=$((N_NOMI_F + 1))
	N_COGNOMI=$(cat -n $FOLDER"liste/cognomi.txt" | wc -l)
	N_COGNOMI=$((N_COGNOMI + 1))
	N_COMUNE=$(cat -n $FOLDER"liste/fast.txt" | wc -l)
	N_COMUNE=$((N_COMUNE + 1))
	N_PATOL=$(cat -n $FOLDER"liste/malattie_elenco.txt" | wc -l)
	N_PATOL=$((N_PATOL + 1))
	j=0
	while [ $j -le $1 ]
	do
		NUM=$RANDOM
		ANNO=$((2014 - NUM%80))
		NUM=$RANDOM
		MESE=$((NUM%12 + 1))
		NUM=$RANDOM
		GIORNO=$((NUM%28 + 1))
		NUM=$RANDOM
		MF=$((NUM%2))
		NUM=$RANDOM
		if [ $MF -eq 0 ]; then
			RND=$((1 + NUM%N_NOMI_M))
			NOME=$(cat -n $FOLDER"liste/nomi_m.txt" | grep -w "$RND" | awk '{ print tolower($2) }')
			MF='M'
		else
			RND=$((1 + NUM%N_NOMI_M))
			NOME=$(cat -n $FOLDER"liste/nomi_f.txt" | grep -w "$RND" | awk '{ print tolower($2) }')
			MF='F'
		fi
		NUM=$RANDOM
		CGN=$((NUM%N_COGNOMI + 1))
		COGNOME=$(cat -n $FOLDER"liste/cognomi.txt" | grep -w "$RND" | awk '{ print tolower($2), tolower($3) }')
		NUM=$RANDOM
		RND=$((1 + NUM%N_COMUNE))
		COMUNE=$(cat -n $FOLDER"liste/fast.txt" | grep -w "$RND" | awk '{ print $2 }')
		CODFSC=$(./cf "$COGNOME" "$NOME" "$GIORNO" "$MESE" "$ANNO" "$MF" "$COMUNE" "$FOLDER_COMUNI" )
		if [ ${#CODFSC} -eq 16 ]; then
			echo "$CODFSC " >> tmp4324242.txt
			j=$((j + 1))
			printf "\rGenero codici fiscali: $j of $1 [$CODFSC]  " >&2
		fi
	done
	printf "\n" >&2;
	DATA=$(cat $MY_TEMP)
	rm -f $MY_TEMP
	array=($DATA)
	j=0
	for D in $DATA
	do
		if [ ${#D} -lt 16 ]; then
			continue
		fi
		NUM=$RANDOM
		FRIEND=$((NUM%$1 + 1))
		NUM=$RANDOM
		RND=$((NUM%N_PATOL + 1))
		NUM=$RANDOM
		CHS=$((NUM%4))
		MALATTIA=""
		IMMUNITA=""
		if [ $CHS -eq 0 ]; then
			MALATTIA=$(cat -n $FOLDER"liste/malattie_elenco.txt" | grep -w "$RND" | awk '{ print tolower($2), tolower($3), tolower($4), tolower($5), tolower($6), tolower($7), tolower($8), tolower($9) }' | xargs)
		elif [ $CHS -eq 1 ]; then
			IMMUNITA=$(cat -n $FOLDER"liste/malattie_elenco.txt" | grep -w "$RND" | awk '{ print tolower($2), tolower($3), tolower($4), tolower($5), tolower($6), tolower($7), tolower($8), tolower($9) }' | xargs)
		elif [ $CHS -eq 2 ]; then
			MALATTIA=$(cat -n $FOLDER"liste/malattie_elenco.txt" | grep -w "$RND" | awk '{ print tolower($2), tolower($3), tolower($4), tolower($5), tolower($6), tolower($7), tolower($8), tolower($9) }' | xargs)
			NUM=$RANDOM
			RND=$((NUM%N_PATOL + 1))
			IMMUNITA=$(cat -n $FOLDER"liste/malattie_elenco.txt" | grep -w "$RND" | awk '{ print tolower($2), tolower($3), tolower($4), tolower($5), tolower($6), tolower($7), tolower($8), tolower($9) }' | xargs)
		fi
		FRIEND=${array[$FRIEND]}
		NUM=$RANDOM
		NUM=$((NUM%10 + 1))
		if [ ! -z "$FRIEND" ]; then 
			echo "$D;$MALATTIA;$IMMUNITA;1;$FRIEND;$NUM" >> $2
		else
			echo "$D;$MALATTIA;$IMMUNITA" >> $2
		fi
		j=$((j + 1))
		printf "\rGenero il csv casualmente: $j of $1 " >&2
	done
	printf "\n" >&2;
	END_TIME=`date +%s`
    ELAPSED=`expr $END_TIME - $START_TIME || true`
	ELAPSED=$(printf "%02d:%02d:%02d" $((ELAPSED/3600)) $((ELAPSED/60%60)) $((ELAPSED%60)))
    echo "Fatto! (Tempo: $ELAPSED)"
else
	echo "$0 <num> <file>"
fi
