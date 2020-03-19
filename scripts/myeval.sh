echo "## Generating conf files"
python3 scripts/gen_conf.py --block-size 400 --iter 4 --pace-maker dummy
echo "## Generating quorums.json file"
echo "## Starting replicas"
rm -rf a_n && scripts/run.sh start a_n
echo "## Starting clients"
rm -rf a_c && scripts/run_client.sh start a_c
