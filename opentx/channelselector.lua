--
-- VTX Channel Selector
-- Author: Markus Wilhelm
--

-- number of global variable, change if needed
local gvID = 0

local channel

local function drawChannel()
	lcd.clear()
	lcd.drawText(10,20,"RB Channel:",DBLSIZE)
	lcd.drawNumber(135,20,channel,DBLSIZE)
	
	lcd.drawText(10,50,"[EXIT]: Exit")
	lcd.drawText(80,50,"[ENTER]: Save")
end

local function updateChannel(newChannel)
	channel= newChannel
	drawChannel()
end

local function commitChannel()
	-- convert channel to value range -100 .. 100 for mixer weight
	-- steps of 25% , center channel with 12%
	local gvValue= -100 + (channel-1) * 25 + 12
	model.setGlobalVariable(gvID,0,gvValue)
end

local function readChannelFromGV()
	-- convert value range of -100 .. 100 to channel index 1-8
	local gvValue= model.getGlobalVariable(gvID, 0)
	if gvValue < -100 then
		gvValue= -100
	elseif gvValue > 100 then
		gvValue= 100
	end
	return math.floor((gvValue + 100) / 25) + 1
end

local function init()
	channel= readChannelFromGV()
	drawChannel()
end

local function run(event)
  if event == EVT_PLUS_BREAK then
	if channel < 8 then
		updateChannel(channel + 1)
	end
  elseif event == EVT_MINUS_BREAK then
	if channel > 1 then
		updateChannel(channel - 1)
	end
  elseif event == EVT_EXIT_BREAK then
    return 1
  elseif event == EVT_ENTER_BREAK then
    commitChannel()
	return 1
  end
  return 0
end

return{run=run, init=init}
