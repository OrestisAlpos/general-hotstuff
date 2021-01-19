TAG=HOTSTUFF_AUTO_GEN_CLIENT

curl -X DELETE \
-H "Content-Type: application/json" \
-H "Authorization: Bearer $TOKEN" \
"https://api.digitalocean.com/v2/droplets?tag_name=${TAG}"
