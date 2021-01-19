TAG=HOTSTUFF_AUTO_GEN_CLIENT

ret=$(curl -X GET \
-H "Content-Type: application/json" \
-H "Authorization: Bearer $TOKEN" \
"https://api.digitalocean.com/v2/droplets?tag_name=${TAG}&per_page=100")

jq -r ".droplets[] | .networks | .v4[0] | .ip_address" <<< $ret > conf/clients.txt
