"""
This global dictionary contains MSP account details used for automated tests
Accounts with STS_ prefix are used for Staging Masters
Accounts with Prod_ prefix are used for Production Masters

"""

RESOURCES = {
    "AMAZON_ACCOUNT":{
               "name": "matthew_scanlan@bose.com",
               "account_type": "MSP",
               "provider": "AMAZON",
               "secret": "eyJyZWZyZXNoX3Rva2VuIjogIkF0enJ8SXdFQklBZmdSRnE1dlJpVlR6TUp4UzZIbG9zTXlJYm02WEVUQU1WcnZkcms0ZWpCLTNUeUwtVjZBUUtZODJnLWd4cERvUWNyYk5LZ3RXNlo4QXpPTnotNGxwSDR5ZUpsYXlVRmJINmxha2tMdUhmRjFBaEN5c05kTGRhLUdJUXZQNDExZ3NBLWJhOFNHeUpxSXI0Wi15VUgtYmRwOFFCNDZ6UktXWENVNEdacGdqYjR3X01oVlhXV0NkUHNyaEtQa3RRc3JCbldCYjR3S2ZzMkNsbjZHaFF0Z29CRWl4LS1GU0RZQTVZVzdUZmNGei0zT0dhSmV5UU5hSzJGbDR4Y3VDVWp1U2NRQjVnZXByWXl4YUhRUE1NMS1FQXpXa2l3YUpVbHREc29SZUdWMjFJNThRRl9qWUpiU3ZaOW1ReHgtSnlYUzlucGdldEVtbGhFVXlZb3FoekU3aTJlUEoyNmJoZmozZW1uajY5anB0TjBycEs1UXVMc3pTXzFoX0lPODRndVJBa0Z3STk1V1Q2UDRqZjJXemZxZDl2UkVCbjVFZTZnaVBPUjdybjAxLW1uRjdhQi1lVVA5RERvN1dPc3QtdDA3UEE5QUZNdDliVGk0LVFCZWpkZWh6NkdBeHQwQy1VUGREcGRGejF4ZldoQmNqMWQwZHcwNTFEQWtnZ2ZiaG5GT24zU2ZkbVE1al9rakFfUEowbFZuVU9kIiwgInNpdGVfaWQiOiAiM2U4MDNlZGEtNjA5Yi00ZDEzLTlmOTgtN2Q0NDAzYzkzMDUwIn0=",
               "site_id": "5c0206c8-9731-4f1b-bb8b-811e9ed677a7",
               "provider_account_id": "matthew_scanlan@bose.com",
               "bose_person_id": "f203f860-e421-4c9f-b0de-4d0dddac4ec3",
               "account_id": "f30e28f6-2f4d-4ac9-9d96-fcacb8b06d60",
               "port": 30035,
               "secret_type": "token"},

    "TUNEIN_ACCOUNT":{
        "name": "",
        "account_type": "MSP",
        "provider": "TUNEIN",
        "secret":"eyJzZXJpYWwiOiAiZjQzNjRhMjUtMWE3ZC00ODRmLTgyNjUtYjc4YWU1ZjliMThmIn0=",
        "provider_account_id": "hepdauto_01@bose.com",
        "account_id": "b84f819a-bf22-484e-bcbb-6b576a5fcf12",
        "secret_type": "token"}
}
