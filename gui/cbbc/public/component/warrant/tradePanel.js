import '/component/warrant/tradePanel.css'

class TradePanel extends React.Component {
  static propTypes = {
    fields: PropTypes.object,
    fieldsOriginal: PropTypes.object,
    prefix: PropTypes.string,
    totalNo: PropTypes.number,
    // curNo: PropTypes.number,
    curModule: PropTypes.string,
    lang: PropTypes.string,
    setFields: PropTypes.func,
    setFieldsOriginal: PropTypes.func,
    setCurModule: PropTypes.func,
    setCurNo: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    
    this.state = {}
    for(var module of Object.keys(this.props.fields)) {
      this.state[module] = {}
      for (var i=0; i<this.props.totalNo; i++)
        this.state[module][this.props.prefix+i] = {data: {}, error: {}, result: null}
    }
    
    this.handleChangeModule = this.handleChangeModule.bind(this)
    this.handleChangeNum = this.handleChangeNum.bind(this)
    
    this.handleChange = this.handleChange.bind(this)
    this.handleStart = this.handleStart.bind(this)
    this.handleDelete = this.handleDelete.bind(this)
    this.handleOnMouseOver = this.handleOnMouseOver.bind(this)
  }
  
  shouldComponentUpdate(nextProps, nextState) {return true}
  
  componentDidMount() {
    const render = (res) => {
      var data = JSON.parse(res)
      // 买卖單
      if(data.action=='on_order') {
        var id = data.ref
        var order = data.order
        var key = data.key
        var total = formatNumber(order.match_price)*formatNumber(order.match_quantity)
        var status = order.status.toLowerCase().replace("order_status::", "")
        var side = order.side.toLowerCase().replace("order_side::", "")
        var reject_reason = order.reject_reason
        console.log(id, total, status, side, reject_reason)

        // 更新状态
        if(key in this.state) {
          var obj2 = Object.assign({}, this.state[key])
          obj2[id].result = status
          this.props.fields[this.props.curModule].map((field, index) => {
            if(data.hasOwnProperty(field.name))
              if(field.valid=="number")
                obj2[id].data[field.name] = formatNumber(data[field.name]).toFixed(3)
              else
                obj2[id].data[field.name] = data[field.name]
          })
          obj2[id].data.wtype="BULL"
          obj2[id].data.action="AUTO"
          this.setState({[key]: obj2})
        }
      }
      
      // 选轮策略
      else if (data.action=='algo' && "data" in data && data.data.length>0) {
        var arr = Object.keys(this.props.fields)
        for (var i=0; i<data.data.length; i++){
          var algo = data.data[i]
          // 下拉選單
          if(!arr.includes(data.data[i]))
            arr.unshift(algo)
          // 重新渲染
          if(!(algo in this.props.fields)) {
            var _fields = this.props.fields
            _fields[algo] = []
            // 初始策略
            if(algo.indexOf('into_algo')>=-1) {
              _fields[algo] = init_algo
              this.props.setCurModule(algo)
              this.props.setFields(_fields)
              // 初始fields original
              var fieldsOriginal = Object.assign({}, this.props.fieldsOriginal)
              fieldsOriginal[algo] = _fields[algo].map(a => a.name);
              this.props.setFieldsOriginal(fieldsOriginal)
              // 初始儲存data的state
              var obj = Object.assign({}, this.state)
              obj[algo] = {}
              for (var i=0; i<this.props.totalNo; i++)
                obj[algo][this.props.prefix+i] = {data: {}, error: {}, result: null}
              this.setState(obj)
            }
          }
        }
      }
      else {
        // console.log(data)
      }
    }
    initWebsocket(render)
    sendWebsocket('{"type":"list_algo"}')
    this.props.setCurNo(10)
  }
  
  componentDidUpdate() {}
  
  componentWillUnmount() {}
  
  componentDidCatch() {}
  
  static getDerivedStateFromError(error) {
    return { hasError: true };
  }
  
  handleChangeModule(event) {
    event.preventDefault()
    var curModule = event.target.value
    this.props.setCurModule(curModule)
  }
  
  handleChangeNum(event) {
    event.preventDefault()
    this.props.setCurNo(event.target.value)
  }
  
  handleChange(event) {
    event.preventDefault()
    var id = event.target.attributes.getNamedItem('data-id').value
    var {name, value} = event.target
    var curModule = this.props.curModule

    var obj = Object.assign({}, this.state[curModule])
    obj[id].data[name] = formatInput(value)
    this.setState({[curModule]: obj})
  }
  
  handleStart(event) {
    event.preventDefault()
    var id = event.target.attributes.getNamedItem('data-id').value
    var curModule = this.props.curModule
    var fields = this.props.fields[curModule]
    var fieldsOriginal = this.props.fieldsOriginal[curModule]
    var lang = this.props.lang
    
    var obj = Object.assign({}, this.state[curModule])
    obj[id].error = validate2(lang, obj[id], fields)
    this.setState({[curModule]: obj})
    
    if(Object.keys(obj[id].error).length==0) {
      var command = formatCommand(id, obj[id].data, fields, fieldsOriginal).join('|')
      var _command = '{"type":"algo_command", "key": "'+curModule+'", "command": "'+command+'"}'
      console.log(_command)
      sendWebsocket(_command)
    }
  }
  
  handleDelete(event) {
    event.preventDefault()
    var id = event.target.attributes.getNamedItem('data-id').value
    var temp = this.state[this.props.curModule][id]
  }
  
  handleOnMouseOver(event) {
    event.preventDefault()
    event.target.focus()
  }
  
  render() {
    let rows = []
    let curModule = this.props.curModule
    if(curModule in this.state) {
      for (let i=0; i<this.props.curNo; i++) {
      let id = this.props.prefix+i
        rows.push(
          <form 
            key={id}
            id={id}
            className="field_row"
            methord="POST">
              <TradeRow
                key={id}
                id={id}
                fields={this.props.fields[curModule]}
                state={this.state[curModule][id]}
                handleChange={this.handleChange}
                handleOnMouseOver={this.handleOnMouseOver}
                handleStart={this.handleStart}
                handleDelete={this.handleDelete}
              />
          </form>
        )
      }
    }
    return (
      <React.Fragment>
        <ModuleSelector
          lang={this.props.lang}
          curModule={this.props.curModule}
          module={Object.keys(this.props.fields)}
          handleChangeModule={this.handleChangeModule}
        />
        <NumSelector 
          lang={this.props.lang}
          curNo={this.props.curNo}
          totalNo={this.props.totalNo}
          handleChangeNum={this.handleChangeNum}
        />
        <Header 
          fields={this.props.fields[this.props.curModule]}
        />
        {rows}
      </React.Fragment>
    )
  }
}