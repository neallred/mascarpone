curl -X DELETE --data "{\"domain\": \"adsfdaffda.fi\"}" -H "Content-type: application/json" localhost:8080
curl -X POST --data "{\"domain\": \"adsfdaffda.fi\", \"redirect\": \"127.0.0.1\" }" -H "Content-type: application/json" localhost:8080
curl -X PUT --data "{\"newDomain\": \"adsfdaffda.fi\", \"domain\": \"adsfdaffda.fi\", \"redirect\": \"127.0.0.1\" }" -H "Content-type: application/json" localhost:8080
