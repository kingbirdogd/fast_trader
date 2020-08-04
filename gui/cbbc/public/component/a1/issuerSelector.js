class IssuerSelector extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    data2: PropTypes.number,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleChange = this.handleChange.bind(this)
    this.handleAction = this.handleAction.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleChange(event) {
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.issuer)
    obj.curIssuer = event.target.value
    obj.feedback = null
    obj.responseResult = null
    this.props.setStates({issuer: obj})
  }
  
  handleAction() {
    var action = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var issuer = states.issuer.curIssuer
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'selectissuer', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), issuer: issuer, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {select: 'Select', remove: 'Remove', selected: 'Selected'},
      sc: {select: '选择', remove: '删除', selected: '已选择'},
      tc: {select: '選擇', remove: '刪除', selected: '已選擇'}
    }
    return text[lang]
  }
  
  render() {
    var {curIssuer, curState, feedback, responseResult, selected} = this.props.data
    var text = this.getText(this.props.lang)
    
    // 
    if (!curIssuer) var selectedValue='default'
    else if (curIssuer) var selectedValue=curIssuer
    
    // 
    var hint = null
    if (feedback)
      hint = <div className="text-danger"> {feedback} : {curState} </div>
    else if (responseResult)
      hint = <div className="text-success"> {responseResult} : {curState} </div>
    
    // 
    var issuerList = {
      BI: {sc: '中銀', tc: '中银', en: '中银'},
      BP: {sc: '法巴', tc: '法巴', en: '法巴'},
      CS: {sc: '瑞信', tc: '瑞信', en: '瑞信'},
      CT: {sc: '花旗', tc: '花旗', en: '花旗'},
      EA: {sc: '東亞', tc: '東亞', en: '東亞'},
      GJ: {sc: '国君', tc: '國君', en: '国君'},
      GS: {sc: '高盛', tc: '高盛', en: '高盛'},
      HS: {sc: '汇丰', tc: '匯豐', en: '汇丰'},
      HT: {sc: '海通', tc: '海通', en: '海通'},
      JP: {sc: '摩通', tc: '摩通', en: '摩通'},
      MB: {sc: '麦银', tc: '麥銀', en: '麦银'},
      MS: {sc: '摩利', tc: '摩利', en: '摩利'},
      SC: {sc: '渣打', tc: '渣打', en: '渣打'},
      SG: {sc: '法兴', tc: '法興', en: '法兴'},
      UB: {sc: '瑞银', tc: '瑞銀', en: '瑞银'},
      VT: {sc: '瑞通', tc: '瑞通', en: '瑞通'}
    }
    var optionHTML = []
    optionHTML.push(<option value="default" key='issuer_default' disabled>Select Issuer</option>)
    for (const [k, v] of Object.entries(issuerList)) {
      var isSelect = '', style = ''
      if (selected && selected.includes(k))
        isSelect = text.selected, style = 'text-success'
      optionHTML.push(
        <option value={k} key={'issuer_'+k} className={style}>
          {k} ({v[lang]}) {isSelect}
        </option>)
    }
    
    // 
    if (this.props.data2 == 1 || curIssuer === null) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
    else if (selected && selected.includes(curIssuer)) {
      var btnSelectStyle = 'btn-secondary', btnSelectIsDisable = true
      var btnRemoveStyle = 'btn-danger', btnRemoveIsDisable = false
    }
    else {
      var btnSelectStyle = 'btn-success', btnSelectIsDisable = false
      var btnRemoveStyle = 'btn-secondary', btnRemoveIsDisable = true
    }
      
    return(
      <div className='row'>
      <div className="col-12 col-sm-6 col-md-3 mb-3">
      
      <div className="form-group">
        <select className="form-control" id="issuerSelector" onChange={this.handleChange} value={selectedValue}>
          {optionHTML}
        </select>
      </div>
      
      <button
        name="select"
        type="button"
        className={classNames('btn btn-sm', btnSelectStyle)}
        disabled={btnSelectIsDisable}
        onClick={this.handleAction}>
          {text.select}
      </button>
      
      <button
        name="remove"
        type="button"
        className={classNames('btn btn-sm', btnRemoveStyle)}
        disabled={btnRemoveIsDisable}
        onClick={this.handleAction}>
          {text.remove}
      </button>
      
      {hint}
        
      </div>
      </div>
    )
  }
}