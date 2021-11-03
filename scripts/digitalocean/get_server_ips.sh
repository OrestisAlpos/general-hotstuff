TAG=HOTSTUFF_AUTO_GEN_SERVER

ret=$(curl -X GET \
-H "Content-Type: application/json" \
-H "Authorization: Bearer $TOKEN" \
"https://api.digitalocean.com/v2/droplets?tag_name=${TAG}&per_page=100")

# jq -r ".droplets[] | .networks | .v4[1], .v4[0] | .ip_address" <<< $ret > conf/nodeips.txt
jq -r '.droplets[] | .networks |{ public: .v4[1].ip_address, private: .v4[0].ip_address } | "\(.public) \t \(.private)" '<<< $ret > conf/replicas.txt
jq -r '.droplets[] | .networks |{ public: .v4[1].ip_address, private: .v4[0].ip_address } | "\(.private)" '<<< $ret > conf/replicas_inter.txt
jq -r '.droplets[] | .networks |{ public: .v4[1].ip_address, private: .v4[0].ip_address } | "\(.public)" '<<< $ret > conf/replicas_exter.txt