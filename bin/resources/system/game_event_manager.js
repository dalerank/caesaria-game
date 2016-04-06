function EventManager()
{
		this.bindstack = {};
		this.errors = 0;

		this.diagnose = function(verbose)
		{
			var count = 0;
			for (event in this.bindstack)
			{
				var localCount = this.bindstack[event].length;
				if (verbose >= 1)
				{
					console.log("Event " + event + " has " + localCount + " bound functions");
					if (verbose >= 2)
					{
						for (var i = 0; i < localCount; i++)
						{
							console.log("\t function:" + this.bindstack[event][i]);
						}
					}
				}

				count += localCount;
			}
			console.log("Total number of event-function binds is " + count);
		},

		this.dispatchEvent = function(eventName,data)
		{
			this.runStack(eventName, data);
		},

		this.bindEvent = function(eventName, functionName)
		{
			if(this.bindstack[eventName] == undefined)
			{
				this.bindstack[eventName] = [functionName];
			}
			else
			{
				this.bindstack[eventName].push(functionName);
			}
		},

		this.unbindEvent = function(eventName, functionName)
		{
			var found = false;
			var exist = false;

			if(this.bindstack[eventName] != undefined)
			{
				for(var i=0; i<this.bindstack[eventName].length; ++i)
				{
					if(String(this.bindstack[eventName][i])== String(functionName))
					{
						found = true;
						this.bindstack[eventName].splice(i,1);
					}
				}
				if(this.bindstack[eventName].length == 0 || functionName == undefined)
				{
					found = true;
					delete this.bindstack[eventName];
				}
			}
			else
			{
				exist = true;
			}

			if(found == false && exist == false)
			{
				this.errors +=1;
			}
		},

		this.runStack = function(event, data)
		{
			if(this.bindstack[event] != undefined)
			{
				var stackLength = this.bindstack[event].length - 1;
				for(var i=stackLength; i>=0; i--)
				{
					if(typeof this.bindstack[event][i] == "function")
					{
						this.bindstack[event][i](data);
					}
				}
			}
		}
};

game.eventmgr = new EventManager();
