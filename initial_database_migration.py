import sqlite3
import sys
import random
import string


def id_generator(size=6, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for _ in range(size))

if __name__ == "__main__":
    database_name = 'example.db'
    if len(sys.argv)>1:
        database_name = sys.argv[1]

    print('database name: ', database_name)
    conn = sqlite3.connect(database_name)

    c = conn.cursor()

    # Create table
    c.execute('''CREATE TABLE IF NOT EXISTS users
                 (email text PRIMARY KEY, nickname text, is_active integer, password text)''')

    c.execute("INSERT INTO users VALUES ('email_one','nickname_one',0,'secret_one')")

    id = id_generator()
    is_active = id_generator(size=1, chars='01')
    c.execute(f"INSERT INTO users VALUES ('email_{id}','nickname_{id}',{is_active},'secret_{id}')")

    # Save (commit) the changes
    conn.commit()

    conn.close()

        