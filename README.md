# ftn
a wrapper for tonos-cli

## About
If you were to ask, why would I do need that wrapper? 
I would say that you probably don't need it.
I can only speak for myself, and I find it very useful.
Just look at that. 
Suppose, your account number is -1:1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef
and let's say you want to know what your account balance is, then you need to make a command like that
```
# at first make sure you run it from directory with the proper tonlabs-cli.conf.json
tonos-cli account -1:1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef
```
and it will give you a full info about you account
or you could just type
```
ftn balance
```
of course you can find out the balance of any other account
```
ftn balance -1:3333333333333333333333333333333333333333333333333333333333333333
```

In addition, participating in the elections is more flexible with the ftn wrapper.
You need to put in your cron running of "validator_msig.sh <STAKE>"
And if you want to change the stake size, than you have to edit your cron job.
With the ftn you can do the following:
1. Set the stake size you want
```
ftn setstake <STAKE>
```
2. Put into cron
```
ftn electionrun
```
That's all! It will use the stake specified by 'setstake' subcommand. 
And if you want to change the stake size, just type
```
ftn setstake <NEW STAKE>
```

You faced an error while sending the stake for the current elections and now you can't send it once again, because validator_msig.sh says it is already submitted?
Ok! Use 'extrastake' subcommand:
```
ftn extrastake <STAKE>
```

And if you wish, you can only take an unlocked recover from past elections without calling validator_msig.sh at all
```
ftn recover
```

Wanna know your current adnl?
```
ftn adnl
```

Etc, etc...
As you can see, ftn simplifies usecases. While using tonos-cli you have to write you account address, abi, keys, etc. But ftn wrapper takes care about that stuff for you.

Of course, everyone can write their own set of scripts to do the same, but ftn is one simple and flexible script. And it's already here.


## Installation
Please make sure that your user is sudoers and can use sudo.
To install this wrapper run the following command
```sh
cd ~
git clone https://github.com/user529/ftn
cd ftn
chmod 755 ftnautoconf
./ftnautoconf
```
Now you can use it! Try
```sh
ftn help
```
If you want to tweak the ftn configuration, use ~/ftn.conf

## Update
If you already have ftn installed and want to keep it up to date, just follow these a few steps
```sh
cd ~/ftn
git pull origin
```