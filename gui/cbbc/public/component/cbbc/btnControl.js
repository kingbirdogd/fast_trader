class BtnControl extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    type: PropTypes.string,
    data: PropTypes.object,
    lang: PropTypes.string,
    isHide: PropTypes.object,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.btn = {
      set: {style: null, isDisabled: true},
      start: {style: null, isDisabled: true},
      pause: {style: null, isDisabled: true},
      stop: {style: null, isDisabled: true}
    }
    this.handleAction = this.handleAction.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    var btn = state.btn
    var d = props.data
    
    // 是否错误数据，禁止按钮
    var isWrongData = false
    for (const [k, v] of Object.entries(d))
      if(v.hasOwnProperty('feedback') && v.feedback.toString().length>0)
        isWrongData = true
    // 是否已填滿所有選項
    var isFillAllData = true
    for (const [k, v] of Object.entries(d))
      if (v.hasOwnProperty('value') && v.value.toString().length<=0)
        isFillAllData = false
    
    var d = props.data.status
    // 有錯誤
    if (isWrongData)
      btn.set.isDisabled = true, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    // 如填滿, 没有錯誤, 可剛填錯導致所有btn被禁止
    else if (isFillAllData && !isWrongData && !d.isStart && !d.isPause && !d.isSet && !d.isStop)
      btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    // 如沒填滿, 停留set狀態
    else if (!isFillAllData && (!d.isStart===true && !d.isPause===true))
      btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    // 如填滿, 等待start, 不能返回set狀態, 也不能再更改data
    else if (isFillAllData && d.isSet===true)
      btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    // 如填滿&啟動start, 等待pause/stop
    else if (isFillAllData && d.isStart===true)
      btn.set.isDisabled = true, btn.start.isDisabled = true, btn.pause.isDisabled = false, btn.stop.isDisabled = false
    // 等待pause, 返回set狀態, 重新開始
    else if (isFillAllData && d.isPause===true)
      btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = false
    // 等待stop, 可返回set狀態更改Data, 也可start重新掛盤
    else if (d.isStop===true && d.needToSetAgain==false)
      btn.set.isDisabled = false, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    // 如stop后更改了data，必须重新set盘
    else if (d.isStop===true && d.needToSetAgain==true)
      btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
    
    // 样式
    btn.set.style = (d.isSet ? 'btn-primary' : 'btn-secondary')
    btn.start.style = (d.isStart ? 'btn-success' : 'btn-secondary')
    btn.pause.style = (d.isPause ? 'btn-warning' : 'btn-secondary')
    btn.stop.style = (d.isStop ? 'btn-danger' : 'btn-secondary')
    
    state.btn = btn
    return state
  }
  
  handleAction() {
    var name = event.target.name
    var no = this.props.no
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.cells[no].action)
    var obj2 = $.extend(true, [], states.cellsConfig)
    
    // 校验数据
    var isError = false
    for (const [k, v] of Object.entries(obj)) {
      if(v.hasOwnProperty("valid")) {
        v.feedback = validate(this.props.lang, v.valid, v.value, true)
        if (v.feedback.length>0)
          isError = true
      }
    }

    /*// 更新按钮状态 （已放弃，采用後置检查，然后更新状态）
    if(!isError)
      obj.status.isSet = false, obj.status.isStart = false, obj.status.isPause = false, obj.status.isStop = false, obj.status[name] = true*/
    
    states.cells[no].action = obj
    
    // 清除setting & action 样式
    if (name == 'isStop') {
      states.cells[no].setting.inout.responseResult = ''
      states.cells[no].setting.wintick.responseResult = ''
      states.cells[no].setting.inlvl.responseResult = ''
      states.cells[no].setting.stoplost.responseResult = ''
      states.cells[no].setting.ptrange.responseResult = ''
      
      states.cells[no].action.issuerSize.responseResult = ''
      states.cells[no].action.quantity.responseResult = ''
    }
    
    var version = 2
    // 接口 v1
    if (version==1) {
      // Set指令
      if(!isError && name == 'isSet') {
        var format1 = (val) => {return parseFloat(formatInputUnit(val, true))}
        var command = ['set', obj2[no].type, obj.symbol.value, obj.code.value, format1(obj.issuerSize.value), formatLongV2(format1(obj.quantity.value)), formatLongV2(obj.spread.value), states.prefix+no]
        var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
      // Start指令
      else if(!isError && name == 'isStart') {
        var command = ['start', obj.code.value, states.prefix+no]
        var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
      // pause指令
      else if(!isError && name == 'isPause') {
        var command = ['pause', obj.code.value, states.prefix+no]
        var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
      // stop指令
      else if(!isError && name == 'isStop') {
        var command = ['stop', obj.code.value, states.prefix+no]
        var command1 = {type: "algo_command", "key": states.modules[this.props.type], command: command.join('|')}
        sendWebsocket(JSON.stringify(command1))
      }
    }
    // 接口 v2
    else if (version==2) {
      // Set指令
      if(!isError && name == 'isSet') {
        var format1 = (val) => {return parseFloat(formatInputUnit(val, true))}
        var command1 = {
          cmd: "set",
          wtype: obj2[no].type,
          symbol: obj.symbol.value,
          warrant_code: parseInt(obj.code.value),
          issuersize: format1(obj.issuerSize.value),
          quantity: formatLongV2(format1(obj.quantity.value)),
          spread: formatLongV2(obj.spread.value),
          delta: (obj.delta.value).toString(),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[this.props.type]
        }
        sendWebsocket(JSON.stringify(command1))
      }
      // Start指令
      else if(!isError && name == 'isStart') {
        var command1 = {
          cmd: "start",
          action: "start",
          warrant_code: parseInt(obj.code.value),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[this.props.type]
        }
        sendWebsocket(JSON.stringify(command1))
      }
      // pause指令
      else if(!isError && name == 'isPause') {
        var command1 = {
          cmd: "pause",
          action:"pause",
          warrant_code: parseInt(obj.code.value),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[this.props.type]
        }
        sendWebsocket(JSON.stringify(command1))
      }
      // stop指令
      else if(!isError && name == 'isStop') {
        var command1 = {
          cmd: "stop",
          action: "stop",
          warrant_code: parseInt(obj.code.value),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[this.props.type]
        }
        sendWebsocket(JSON.stringify(command1))
      }
    }
    
    states.cells[no].action.status.needToSetAgain = false
    this.props.setStates({cells: states.cells})
  }
  
  getText(lang) {
    var text = {
      en: {
        btn: {set: 'Set', start: 'Start', pause: 'Pause', stop: 'Stop'}},
      sc: {
        btn: {set: '设置', start: '开始', pause: '暂停', stop: '停止'}},
      tc: {
        btn: {set: '設置', start: '開始', pause: '暫停', stop: '停止'}}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var btn = this.state.btn
    var isHideStyle = (this.props.isHide.set && this.props.isHide.stop) ? {padding: '0px', margin: '0px', display: 'none'}: {}
    var errMsgStyle = (this.props.isHide.set && this.props.isHide.stop) ? {marginBottom: '32px'}: {}
    return(
    <div className='btn_control'>
      
      {this.props.data.status.result &&
        <div className="text-danger" style={errMsgStyle}> {this.props.data.status.result} </div>
      }
    
      <div className="form-group row row-1" style={isHideStyle}>
        {!this.props.isHide.set &&
          <div className="col-6 col-sm-6 mt-0 mb-1">
            <button 
              name="isSet"
              type="button"
              className={classNames('btn btn-sm btn-block',btn.set.style)}
              disabled={btn.set.isDisabled}
              onClick={this.handleAction}> 
                {text.btn.set}
            </button>
          </div>
        }
        
        {!this.props.isHide.stop &&
          <div className="col-6 col-sm-6 mt-0 mb-1">
            <button
              name="isStop"
              type="button"
              className={classNames('btn btn-sm btn-block',btn.stop.style)}
              disabled={btn.stop.isDisabled}
              onClick={this.handleAction}>
                {text.btn.stop}
            </button>
          </div>
        }
      </div>
      
      <div className="form-group row row-2">
        {!this.props.isHide.start &&
          <div className="col-6 col-sm-6 mt-0 mb-1">
            <button 
              name="isStart"
              type="button"
              className={classNames('btn btn-sm btn-block',btn.start.style)}
              disabled={btn.start.isDisabled}
              onClick={this.handleAction}> 
                {text.btn.start}
            </button>
          </div>
        }
        
        {!this.props.isHide.pause &&
          <div className="col-6 col-sm-6 mt-0 mb-1">
            <button
              name="isPause"
              type="button"
              className={classNames('btn btn-sm btn-block',btn.pause.style)}
              disabled={btn.pause.isDisabled}
              onClick={this.handleAction}> 
                {text.btn.pause}
            </button>
          </div>
        }
      </div>
    </div>
    )
  }
}