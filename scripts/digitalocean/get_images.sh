curl -X GET \
-H "Content-Type: application/json" \
-H "Authorization: Bearer ${TOKEN}" \
"https://api.digitalocean.com/v2/images?private=true"
