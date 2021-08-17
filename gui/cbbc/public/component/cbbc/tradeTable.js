class TradeTable extends React.Component {
  static propTypes = {
    data2: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.handleClick = this.handleClick.bind(this)
    this.state = {}
    this.state.btn = []
  }
  
  componentDidUpdate() {
    
  }
  
  handleClick() {
    var no = event.target.attributes.getNamedItem('data-no').value,
        name = event.target.name,
        states = this.props.getStates(),
        obj = $.extend(true, {}, states.cells[no].action),
        obj2 = $.extend(true, [], states.cellsConfig),
        type = obj2[no].type
        
    if (name == 'show') {
      if (obj2[no].isVisable == true)
        obj2[no].isVisable = false
      else if (obj2[no].isVisable == false)
        obj2[no].isVisable = true
      this.props.setStates({cellsConfig: obj2})
    }
    
    else if (name == 'isSet') {
      var format1 = (val) => {return parseFloat(formatInputUnit(val, true))}
      var command1 = {
        cmd: "set",
        wtype: type,
        symbol: obj.symbol.value,
        warrant_code: parseInt(obj.code.value),
        issuersize: format1(obj.issuerSize.value),
        quantity: formatLongV2(format1(obj.quantity.value)),
        spread: formatLongV2(obj.spread.value),
        delta: (obj.delta.value).toString(),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
    
    else if (name == 'isStart') {
        var command1 = {
          cmd: "start",
          action: "start",
          warrant_code: parseInt(obj.code.value),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[type]
        }
        sendWebsocket(JSON.stringify(command1))
    }
    
    else if (name == 'isPause') {
      var command1 = {
        cmd: "pause",
        action:"pause",
        warrant_code: parseInt(obj.code.value),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
    
    else if (name == 'isStop') {
      var command1 = {
        cmd: "stop",
        action: "stop",
        warrant_code: parseInt(obj.code.value),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
  }
  
  getText(lang) {
    var text = {
      en: {id: 'ID', ucode: 'Underlying', issuer: 'Issuer', code: 'Code', wtype: 'Type', name: 'Name', lotSize: 'Lot size', bid: 'Bid', diffpt: 'Diff Point', ask: 'Ask', action: 'Action', bull: 'Bull', bear: 'Bear', set: 'Set', start: 'Start', pause: 'Pause', stop: 'Stop', show: 'Show', hide: 'Hide'},
      sc: {id: 'ID', ucode: '标的', issuer: '发行人', code: '牛熊证', wtype: '种类', name: '名称', lotSize: '手数', bid: '买入价', diffpt: '打和点', ask: '卖出价', action: '操作', bull: '牛证', bear: '熊证', set: '设置', start: '开始', pause: '暂停', stop: '停止', show: '显示', hide: '隱藏'},
      tc: {id: 'ID', ucode: '正股', issuer: '發行人', code: '牛熊證', wtype: '種類', name: '名稱', lotSize: '手數', bid: '買入價', diffpt: '打和點', ask: '賣出價', action: '操作', bull: '牛證', bear: '熊證', set: '設置', start: '開始', pause: '暫停', stop: '停止', show: '顯示', hide: '隐藏'},
    }
    return text[lang]
  }
  
  render() {
    var no = this.props.no
    var text = this.getText(this.props.lang)
    
    var rows = [], no = 1
    for (var i in this.props.data) {
      var v = this.props.data[i], config = null
      // has code
      if (v.action.code.value != '' &&  v.action.code.value.length >= 5) {
        // get config
        for (var v1 of this.props.data2) {
          if (v.action.code.value == v1.code)
            config = v1
        }
        if (config) {
          // show
          var isShow = '', cssShow = ''
          if (config.isVisable == true)
            isShow = text.show, cssShow = 'btn-secondary'
          else if (config.isVisable == false)
            isShow = text.hide, cssShow = 'btn-light'
          
          // btn
          var d = v.action.status
          var btn = {
            set: {style: null, isDisabled: true},
            start: {style: null, isDisabled: true},
            pause: {style: null, isDisabled: true},
            stop: {style: null, isDisabled: true}
          }
          
          if (!d.isSet===true && !d.isStart===true && !d.isPause===true && !d.isStop===true)
            btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
          if (d.isSet===true)
            btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true
          else if (d.isStart===true)
            btn.set.isDisabled = true, btn.start.isDisabled = true, btn.pause.isDisabled = false, btn.stop.isDisabled = false
          else if (d.isPause===true)
            btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = false
          else if (d.isStop===true)
            btn.set.isDisabled = false, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true
          
          btn.set.style = (d.isSet ? 'btn-primary' : 'btn-secondary')
          btn.start.style = (d.isStart ? 'btn-success' : 'btn-secondary')
          btn.pause.style = (d.isPause ? 'btn-warning' : 'btn-secondary')
          btn.stop.style = (d.isStop ? 'btn-danger' : 'btn-secondary')
          
          var btns = []
          for (const [k, v] of Object.entries(btn)) {
            btns.push(
              <button
                key={k}
                name={'is'+capitalize(k)}
                data-no={i}
                type="button"
                className={classNames('btn btn-sm mr-1', v.style)}
                disabled={v.isDisabled}
                onClick={this.handleClick}>
                  {text[k]}
              </button>
            )
          }

          // html
          rows.push(
            <tr key={'trade_'+i}>
              <td>{no}</td>
              <td>{v.action.code.value}</td>
              <td>{v.info.uname}</td>
              <td>{text[config.type]}</td>
              <td>{v.action.quantity.value}</td>
              <td>{v.wPrice.bid}</td>
              <td>{v.wPrice.diffpt}</td>
              <td>{v.wPrice.ask}</td>
              
              <td>
              <button
                name='show'
                data-no={i}
                type="button"
                className={classNames("btn btn-sm mr-1", cssShow)}
                onClick={this.handleClick}>
                  {isShow}
              </button>
              </td>
              
              <td>{btns}</td>
            </tr>
          )
          no++
        }
      }
    }

    return(
      <div className='row'>
      <div className="col-12">
      <table className="table table-sm table-striped table-light table-trade">
        <colgroup>
          <col span="1" width="50px" />
          <col span="1" width="80px" />
          <col span="1" width="150px" />
          <col span="1" width="80px" />
          <col span="1" width="80px" />
          <col span="1" width="80px" />
          <col span="1" width="80px" />
          <col span="1" width="80px" />
          <col span="1" width="80px" />
          <col span="1" width="200px" />
        </colgroup>
        <thead>
          <tr>
          <th>{text.id}</th>
          <th>{text.code}</th>
          <th>{text.ucode}</th>
          <th>{text.wtype}</th>
          <th>{text.lotSize}</th>
          <th>{text.bid}</th>
          <th>{text.diffpt}</th>
          <th>{text.ask}</th>
          <th>{text.show}</th>
          <th>{text.action}</th>
          </tr>
        </thead>
        <tbody>
          {rows}
        </tbody>
      </table>
      </div>
      </div>
    )
  }
}